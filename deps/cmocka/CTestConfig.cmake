set(UPDATE_TYPE "true")

set(CTEST_PROJECT_NAME "cmocka")
set(CTEST_NIGHTLY_START_TIME "01:00:00 CET")

set(CTEST_DROP_METHOD "http")
set(CTEST_DROP_SITE "mock.cryptomilk.org")
set(CTEST_DROP_LOCATION "/submit.php?project=${CTEST_PROJECT_NAME}")
set(CTEST_DROP_SITE_CDASH TRUE)

