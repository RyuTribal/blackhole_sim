#!/usr/bin/env bash

set -Eeuo pipefail

trap 'echo "✖ error: ${BASH_SOURCE[0]}:$LINENO: ${BASH_COMMAND}" >&2' ERR

BUILD_DIR="${BUILD_DIR:-build}"
RUN_AFTER_BUILD=0
TYPE="Debug"
APP_PATH="${APP_PATH:-bin/${TYPE}/black_hole_simulator}"
FORMAT_AFTER_BUILD=0

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"

cd "$SCRIPT_DIR"

usage() {
  cat <<USAGE
Usage: ${BASH_SOURCE[0]} [Debug|Release|RelWithDebInfo|MinSizeRel] [-r|--run]

Arguments are optional and order-independent:
  Debug|Release|RelWithDebInfo|MinSizeRel  Build type (default: Debug)
  -r, --run                                Run the application after build
  -fmt, --format                           Run ./scripts/format-all.sh before configuring
  -h, --help                               Show this help message
USAGE
}

while [[ $# -gt 0 ]]; do
  case "$1" in
  Debug | Release | RelWithDebInfo | MinSizeRel)
    TYPE="$1"
    ;;
  -r | --run)
    RUN_AFTER_BUILD=1
    ;;
  -h | --help)
    usage
    exit 0
    ;;
  *)
    echo "Unknown option: $1" >&2
    usage
    exit 1
    ;;
  esac
  shift
done

APP_PATH="${APP_PATH:-bin/${TYPE}/black_hole_simulator}"
SHOULD_RUN=$([[ $RUN_AFTER_BUILD -eq 1 ]] && echo "run" || echo "norun")

resolve_executable() {
  local slug="black_hole_simulator"
  local base="$BUILD_DIR"
  local declared="$APP_PATH"
  local candidates=()

  if [[ -n "$declared" ]]; then
    if [[ "$declared" = /* ]]; then
      candidates+=("$declared")
    else
      candidates+=("$base/$declared")
      candidates+=("$declared")
    fi
  fi

  candidates+=("$base/bin/${TYPE}/$slug" "$base/$slug" "$base/bin/$slug" "$base/${TYPE}/$slug")

  for candidate in "${candidates[@]}"; do
    [[ -n "$candidate" ]] || continue
    if [[ -x "$candidate" ]]; then
      echo "$candidate"
      return 0
    fi
  done
  return 1
}

cmake -B "$BUILD_DIR" \
  -DCMAKE_BUILD_TYPE="$TYPE" \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build "$BUILD_DIR" --parallel

echo "Build completed."

case "$SHOULD_RUN" in
run)
  if executable_path="$(resolve_executable)"; then
    echo "Running application: $executable_path"
    "$executable_path"
  else
    echo "Build finished, but executable was not found (checked $APP_PATH and common defaults)." >&2
    exit 1
  fi
  ;;
norun)
  echo "Done."
  ;;
*)
  echo "Build finished — unknown run command: $SHOULD_RUN"
  exit 2
  ;;
esac
