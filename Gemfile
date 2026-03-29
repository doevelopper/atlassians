source "https://rubygems.org"

# Cucumber-Ruby is required because cucumber-cpp currently uses the Wire Protocol.
# Note: cucumber-cpp is not compatible with Cucumber-Ruby 3.x due to a known wire bug.
# Pin to a Ruby 3.0 compatible Cucumber line.
# gem "cucumber", "~> 9.2", ">= 9.2.1"

# *****************************************************************
# * To use the slowhandcuke formatter, simple add                 *
# *   --format 'Slowhandcuke::Formatter'                          *
# * to your cucumber.yml, Rakefile, or command line call          *
# *****************************************************************

group :test do
  # cucumber 9.x is the last version with stable cucumber-wire support
  gem 'cucumber', '~> 9.2'
  gem 'cucumber-wire', '~> 7.0'
  gem 'cucumber_statistics', "=3.0.0"
  gem 'slowhandcuke', '~> 0.0.3'
  # Use version 20.x for compatibility with cucumber 9.x message protocol
  gem 'cucumber-html-formatter', '~> 20.0'
end