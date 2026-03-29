#!/usr/bin/env bash
set -euo pipefail

# ============================================================
# GPG End-to-End Setup Script (fixed)
# - Robust against empty inputs
# - Safe trap when batchfile not set
# - RSA 4096 or ECC (ed25519 + cv25519)
# ============================================================

# ---------------------------
# Configuration (env overrides)
# ---------------------------
GPG_NAME="${GPG_NAME:-}"
GPG_EMAIL="${GPG_EMAIL:-}"
GPG_COMMENT="${GPG_COMMENT:-}"          # optional; will be omitted if empty
GPG_PASSPHRASE="${GPG_PASSPHRASE:-}"    # leave empty to be prompted by pinentry
GPG_EXPIRE="${GPG_EXPIRE:-1y}"          # e.g. 1y, 2y, 0 (never)
GPG_KEYTYPE="${GPG_KEYTYPE:-RSA}"       # RSA or ECC
GPG_KEYLENGTH="${GPG_KEYLENGTH:-4096}"  # only for RSA
GPG_UPLOAD="${GPG_UPLOAD:-false}"
GPG_KEYSERVER="${GPG_KEYSERVER:-hkps://keys.openpgp.org}"
GPG_SET_GIT="${GPG_SET_GIT:-true}"
GPG_DEMO="${GPG_DEMO:-true}"
GPG_HOMEDIR="${GPG_HOMEDIR:-$HOME/.gnupg}"

ok()    { echo -e "\033[32m[OK]\033[0m $*"; }
info()  { echo -e "\033[34m[INFO]\033[0m $*"; }
warn()  { echo -e "\033[33m[WARN]\033[0m $*"; }
err()   { echo -e "\033[31m[ERR]\033[0m $*" >&2; }
need_cmd() { command -v "$1" >/dev/null 2>&1; }

# ---------------------------
# Install GPG if needed
# ---------------------------
install_gpg() {
  if need_cmd gpg; then
    ok "gpg found: $(gpg --version | head -1)"
    return
  fi
  info "gpg not found, attempting install..."
  if [[ "$OSTYPE" == "darwin"* ]]; then
    if ! need_cmd brew; then
      err "Homebrew not found. Install from https://brew.sh/ or install GPG manually."
      exit 1
    fi
    brew update
    brew install gnupg pinentry-mac
    mkdir -p "$GPG_HOMEDIR"; chmod 700 "$GPG_HOMEDIR"
    if ! grep -q "pinentry-program" "$GPG_HOMEDIR/gpg-agent.conf" 2>/dev/null; then
      echo "pinentry-program $(brew --prefix)/bin/pinentry-mac" >> "$GPG_HOMEDIR/gpg-agent.conf"
    fi
    gpgconf --kill gpg-agent || true
  else
    if need_cmd apt; then
      sudo apt update && sudo apt install -y gnupg2 gnupg-agent dirmngr pinentry-curses
    elif need_cmd apt-get; then
      sudo apt-get update && sudo apt-get install -y gnupg2 gnupg-agent dirmngr pinentry-curses
    elif need_cmd dnf; then
      sudo dnf install -y gnupg2 pinentry
    elif need_cmd yum; then
      sudo yum install -y gnupg2 pinentry
    elif need_cmd pacman; then
      sudo pacman -Sy --noconfirm gnupg pinentry
    else
      err "Could not determine package manager. Install GnuPG manually and re-run."
      exit 1
    fi
  fi
  ok "gnupg installed."
}

# ---------------------------
# Gather identity (interactive if not set)
# ---------------------------
gather_identity() {
  if [[ -z "$GPG_NAME" ]]; then
    read -rp "Full name for key (e.g., Jane Doe): " GPG_NAME
  fi
  if [[ -z "$GPG_EMAIL" ]]; then
    read -rp "Email for key (e.g., jane@example.com): " GPG_EMAIL
  fi
  if [[ -z "$GPG_COMMENT" ]]; then
    # Optional; typing 'None' previously caused a literal "None". Let Enter mean empty.
    read -rp "Comment (optional; press Enter to skip): " GPG_COMMENT
  fi

  # Validate mandatory fields
  if [[ -z "$GPG_NAME" ]]; then
    err "Name cannot be empty."
    exit 1
  fi
  if [[ -z "$GPG_EMAIL" ]]; then
    err "Email cannot be empty."
    exit 1
  fi

  info "Using identity: $GPG_NAME <${GPG_EMAIL}>${GPG_COMMENT:+ ($GPG_COMMENT)}"
}

# ---------------------------
# Generate keypair (robust)
# ---------------------------
generate_key() {
  mkdir -p "$GPG_HOMEDIR"
  chmod 700 "$GPG_HOMEDIR"

  # Safe trap even if batchfile is unset
  batchfile=""
  cleanup_batch() { [[ -n "${batchfile:-}" && -f "$batchfile" ]] && rm -f "$batchfile"; }
  trap cleanup_batch EXIT

  if [[ "$GPG_KEYTYPE" == "RSA" ]]; then
    batchfile="$(mktemp)"
    {
      echo "%echo Generating RSA key"
      echo "Key-Type: RSA"
      echo "Key-Length: $GPG_KEYLENGTH"
      echo "Subkey-Type: RSA"
      echo "Subkey-Length: $GPG_KEYLENGTH"
      echo "Name-Real: $GPG_NAME"
      [[ -n "$GPG_COMMENT" ]] && echo "Name-Comment: $GPG_COMMENT"
      echo "Name-Email: $GPG_EMAIL"
      echo "Expire-Date: $GPG_EXPIRE"
      # Only add Passphrase if provided (prevents empty param error)
      if [[ -n "$GPG_PASSPHRASE" ]]; then
        echo "Passphrase: $GPG_PASSPHRASE"
      fi
      echo "%commit"
      echo "%echo done"
    } > "$batchfile"

    # If passphrase provided, allow loopback; otherwise use pinentry
    if [[ -n "$GPG_PASSPHRASE" ]]; then
      gpg --batch --pinentry-mode loopback --gen-key "$batchfile"
    else
      gpg --batch --gen-key "$batchfile"
    fi
  else
    # ECC: ed25519 (sign) + cv25519 (encrypt)
    local uid="${GPG_NAME}${GPG_COMMENT:+ ($GPG_COMMENT)} <${GPG_EMAIL}>"
    if [[ -n "$GPG_PASSPHRASE" ]]; then
      gpg --batch --passphrase "$GPG_PASSPHRASE" --pinentry-mode loopback \
          --quick-gen-key "$uid" ed25519 sign "$GPG_EXPIRE"
      gpg --batch --passphrase "$GPG_PASSPHRASE" --pinentry-mode loopback \
          --quick-add-key "$uid" cv25519 encrypt "$GPG_EXPIRE"
    else
      gpg --quick-gen-key "$uid" ed25519 sign "$GPG_EXPIRE"
      gpg --quick-add-key "$uid" cv25519 encrypt "$GPG_EXPIRE"
    fi
  fi

  ok "Key generated."
}

# ---------------------------
# Get Key ID and Fingerprint
# ---------------------------
get_key_id() {
  local fpr
  fpr="$(gpg --list-keys --with-colons "$GPG_EMAIL" | awk -F: '/^fpr:/ {print $10; exit}')"
  if [[ -z "$fpr" ]]; then
    err "Could not find key fingerprint for $GPG_EMAIL"
    exit 1
  fi
  KEY_FPR="$fpr"
  KEY_ID="$(gpg --keyid-format=long --list-keys "$GPG_EMAIL" | awk '/^pub/{print $2}' | sed -E 's/^.*\///' | head -n1)"
  info "Key Fingerprint: $KEY_FPR"
  info "Key ID: $KEY_ID"
}

# ---------------------------
# Create a revocation certificate
# ---------------------------
make_revocation_cert_old() {
  local revoc="revocation-${KEY_ID}.asc"
  if [[ -f "$revoc" ]]; then
    warn "Revocation certificate already exists: $revoc"
    return
  fi
  info "Creating revocation certificate..."
  if [[ -n "${GPG_PASSPHRASE:-}" ]]; then
    gpg --batch --yes --pinentry-mode loopback --passphrase "$GPG_PASSPHRASE" \
      --output "$revoc" --gen-revoke "$KEY_ID" <<EOF
y
Key compromised
y
EOF
  else
    gpg --batch --yes --output "$revoc" --gen-revoke "$KEY_ID" <<EOF
y
Key compromised
y
EOF
  fi
  chmod 600 "$revoc"
  ok "Revocation certificate saved: $revoc (store it safely, offline)."
}

make_revocation_cert() {
  local revoc="revocation-${KEY_ID}.asc"
  if [[ -f "$revoc" ]]; then
    warn "Revocation certificate already exists locally: $revoc"
    return
  fi

  info "Trying to export existing revocation certificate from keyring..."
  # Many modern gpg versions auto-create a revocation cert; this exports it if present.
  if gpg --output "$revoc" --armor --export-options export-revocation --export "$KEY_ID" 2>/dev/null; then
    if [[ -s "$revoc" ]]; then
      chmod 600 "$revoc"
      ok "Revocation certificate exported: $revoc"
      return
    else
      rm -f "$revoc" 2>/dev/null || true
    fi
  fi

  info "No exportable revocation found. Generating a new one..."
  # Generate in batch mode (non-interactive). Provide reason via stdin.
  if [[ -n "${GPG_PASSPHRASE:-}" ]]; then
    # Use loopback only if a passphrase was provided programmatically
    gpg --batch --yes --pinentry-mode loopback --passphrase "$GPG_PASSPHRASE" \
      --output "$revoc" --gen-revoke "$KEY_ID" <<'EOF'
y
Key compromised
y
EOF
  else
    # No programmatic passphrase; let pinentry handle it (still non-interactive input for reason)
    gpg --batch --yes --output "$revoc" --gen-revoke "$KEY_ID" <<'EOF'
y
Key compromised
y
EOF
  fi

  chmod 600 "$revoc"
  ok "Revocation certificate saved: $revoc (store it safely, offline)."
}

# ---------------------------
# Export public key (ASCII)
# ---------------------------
export_public_key() {
  local pub="pubkey-${KEY_ID}.asc"
  gpg --armor --export "$KEY_ID" > "$pub"
  ok "Public key exported: $pub"
}

# ---------------------------
# Optional: upload to keyserver
# ---------------------------
upload_key() {
  if [[ "$GPG_UPLOAD" != "true" ]]; then
    info "Skipping public key upload. Set GPG_UPLOAD=true to enable."
    return
  fi
  info "Uploading public key to $GPG_KEYSERVER ..."
  gpg --keyserver "$GPG_KEYSERVER" --send-keys "$KEY_ID" || warn "Upload may have failed; some servers require verification."
  ok "Upload attempted. Check your email if the server requires verification."
}

# ---------------------------
# Configure Git signing
# ---------------------------
setup_git() {
  if [[ "$GPG_SET_GIT" != "true" ]]; then
    info "Skipping Git integration. Set GPG_SET_GIT=true to enable."
    return
  fi
  git config --global user.signingkey "$KEY_ID"
  git config --global commit.gpgsign true
  git config --global tag.gpgsign true
  git config --global gpg.program "$(command -v gpg)"
  ok "Git configured to sign commits and tags with $KEY_ID"
}

# ---------------------------
# Demo: encrypt/decrypt and sign/verify
# ---------------------------
demo_ops() {
  if [[ "$GPG_DEMO" != "true" ]]; then
    info "Skipping demo. Set GPG_DEMO=true to enable."
    return
  fi
  local demo_msg="hello_gpg_demo.txt"
  echo "This is a confidential demo message for $GPG_EMAIL at $(date -Is)" > "$demo_msg"

  info "Encrypting to self..."
  gpg --yes --recipient "$KEY_ID" --output "${demo_msg}.gpg" --encrypt "$demo_msg"
  ok "Encrypted: ${demo_msg}.gpg"

  info "Decrypting..."
  gpg --quiet --output "decrypted_${demo_msg}" --decrypt "${demo_msg}.gpg"
  ok "Decrypted: decrypted_${demo_msg}"

  info "Creating detached signature..."
  gpg --yes --detach-sign --output "${demo_msg}.sig" "$demo_msg"
  ok "Signature: ${demo_msg}.sig"

  info "Verifying signature..."
  if gpg --verify "${demo_msg}.sig" "$demo_msg" >/dev/null 2>&1; then
    ok "Signature verified."
  else
    err "Signature verification failed."
    exit 1
  fi
}

summary() {
  cat <<EOF

============================================================
GPG setup complete.

Key ID      : $KEY_ID
Fingerprint : $KEY_FPR
Public key  : pubkey-${KEY_ID}.asc
Revocation  : revocation-${KEY_ID}.asc

Common commands:
  - Show keys:                  gpg --list-keys "$GPG_EMAIL"
  - Show fingerprint:           gpg --fingerprint "$GPG_EMAIL"
  - Export public key (ASCII):  gpg --armor --export $KEY_ID
  - Export secret key (backup): gpg --armor --export-secret-keys $KEY_ID > secret-${KEY_ID}.asc
  - Edit key (extend, add sub): gpg --edit-key $KEY_ID
  - Trust management:           gpg --edit-key $KEY_ID trust

Git tips:
  - Verify signed commit:       git log --show-signature -1
  - Sign a tag:                 git tag -s v1.0 -m "Release v1.0"

Security tips:
  - Store the revocation certificate offline.
  - Back up your secret key (prefer encrypted USB or password manager vault).
  - Consider a hardware key (YubiKey) for touch-to-sign.

============================================================
EOF
}

main() {
  install_gpg
  gather_identity
  generate_key
  get_key_id
  make_revocation_cert
  export_public_key
  upload_key
  setup_git
  demo_ops
  summary
}

main "$@"