#!/usr/bin/env bash
# =============================================================================
# install-ca.sh
# Installs custom PEM / CRT certificates from this directory into the system
# CA trust store.  Call this script during a Dockerfile RUN layer after
# bind-mounting the ca-certificates/ folder.
#
# Usage (Dockerfile):
#   RUN --mount=type=bind,source=.docker/common/ca-certificates,target=/tmp/ca \
#       bash /tmp/ca/install-ca.sh
# =============================================================================
set -euo pipefail

CERT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
STORE="/usr/local/share/ca-certificates"

install -d "${STORE}"

found=0
for cert in "${CERT_DIR}"/*.pem "${CERT_DIR}"/*.crt; do
  [[ -f "${cert}" ]] || continue
  name="$(basename "${cert%.*}").crt"
  echo "  Installing CA certificate: ${name}"
  install -m 644 "${cert}" "${STORE}/${name}"
  found=$(( found + 1 ))
done

if [[ "${found}" -eq 0 ]]; then
  echo "  No custom CA certificates found in ${CERT_DIR} — skipping."
else
  update-ca-certificates
  echo "  ${found} certificate(s) installed."
fi
