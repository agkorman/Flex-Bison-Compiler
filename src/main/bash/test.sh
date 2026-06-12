#! /bin/bash

set -u

BASE_PATH="$(dirname "$0")/../../.."
cd "$BASE_PATH"

GREEN='\033[0;32m'
RED='\033[0;31m'
OFF='\033[0m'
STATUS=0
TMP_DIR="$(mktemp -d)"

if docker compose version >/dev/null 2>&1; then
	COMPOSE_AVAILABLE=1
else
	COMPOSE_AVAILABLE=0
fi

cleanup() {
	rm -rf "$TMP_DIR"
}

validate_generated_compose() {
	local test_name="$1"
	local marker_file="$2"
	local compose_found=0

	for compose in output/*/docker-compose.yml; do
		[ -e "$compose" ] || continue
		[ "$compose" -nt "$marker_file" ] || continue

		compose_found=1
		docker compose -f "$compose" config --quiet >/dev/null 2>&1
		RESULT="$?"
		if [ "$RESULT" == "0" ]; then
			echo -e "    $test_name -> $compose, ${GREEN}and it is${OFF}"
		else
			STATUS=1
			echo -e "    $test_name -> $compose, ${RED}but it is not${OFF} (status $RESULT)"
		fi
	done

	if [ "$compose_found" == "0" ]; then
		STATUS=1
		echo -e "    $test_name, ${RED}but it did not generate a Compose file${OFF}"
	fi
}

trap cleanup EXIT

if [ "$COMPOSE_AVAILABLE" == "1" ]; then
	echo "Compiler should accept, and generate valid Compose files..."
else
	echo "Compiler should accept..."
fi
echo ""

for test in $(ls src/test/c/accept/); do
	MARKER_FILE="$TMP_DIR/$test.marker"
	touch "$MARKER_FILE"
	cat "src/test/c/accept/$test" | ".build/Flex-Bison-Compiler" >/dev/null 2>&1
	RESULT="$?"
	if [ "$RESULT" == "0" ]; then
		echo -e "    $test, ${GREEN}and it does${OFF} (status $RESULT)"
		if [ "$COMPOSE_AVAILABLE" == "1" ]; then
			validate_generated_compose "$test" "$MARKER_FILE"
		fi
	else
		STATUS=1
		echo -e "    $test, ${RED}but it rejects${OFF} (status $RESULT)"
	fi
done
echo ""

echo "Compiler should reject..."
echo ""

for test in $(ls src/test/c/reject/); do
	cat "src/test/c/reject/$test" | ".build/Flex-Bison-Compiler" >/dev/null 2>&1
	RESULT="$?"
	if [ "$RESULT" != "0" ]; then
		echo -e "    $test, ${GREEN}and it does${OFF} (status $RESULT)"
	else
		STATUS=1
		echo -e "    $test, ${RED}but it accepts${OFF} (status $RESULT)"
	fi
done
echo ""

if [ "$COMPOSE_AVAILABLE" == "0" ]; then
	echo "Generated artifacts should be valid Compose files..."
	echo ""
	echo "    (skipped: docker compose is not available in this environment)"
	echo ""
fi

echo "All done."
exit $STATUS
