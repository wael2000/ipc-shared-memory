#!/usr/bin/env bash
set -euo pipefail

# OpenShift runs with a random UID; ensure writable paths
for d in "${OSPL_HOME}/var/log" "${OSPL_HOME}/var/run" "${OSPL_HOME}/var/db" "/dev/shm"; do
  [ -d "$d" ] || mkdir -p "$d"
  chmod -R ug+rwX,o+rwX "$d" || true
done

# Optional: print effective UID/GID for debugging
echo "Running as UID:$(id -u) GID:$(id -g)"

# Start OpenSplice domain service (daemon) and then the app
# If you use single-process mode in your build, you can skip ospl start.
ospl start
trap 'echo "Stopping OpenSplice..."; ospl stop || true' TERM INT EXIT

if [[ "${1:-}" == "run" ]]; then
  exec "${APP_BIN}"
else
  exec "$@"
fi
