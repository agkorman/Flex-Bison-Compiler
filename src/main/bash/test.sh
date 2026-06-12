#! /bin/bash

set -u

BASE_PATH="$(dirname "$0")/../../.."
cd "$BASE_PATH"

GREEN='\033[0;32m'
RED='\033[0;31m'
OFF='\033[0m'
STATUS=0

echo "Compiler should accept..."
echo ""

for test in $(ls src/test/c/accept/); do
	cat "src/test/c/accept/$test" | ".build/Flex-Bison-Compiler" >/dev/null 2>&1
	RESULT="$?"
	if [ "$RESULT" == "0" ]; then
		echo -e "    $test, ${GREEN}and it does${OFF} (status $RESULT)"
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

echo "Generated artifacts should be valid Compose files..."
echo ""

if docker compose version >/dev/null 2>&1; then
	for compose in output/*/docker-compose.yml; do
		[ -e "$compose" ] || continue
		docker compose -f "$compose" config --quiet >/dev/null 2>&1
		RESULT="$?"
		if [ "$RESULT" == "0" ]; then
			echo -e "    $compose, ${GREEN}and it is${OFF}"
		else
			STATUS=1
			echo -e "    $compose, ${RED}but it is not${OFF} (status $RESULT)"
		fi
	done
else
	echo "    (skipped: docker compose is not available in this environment)"
fi
echo ""

echo "All done."
exit $STATUS
