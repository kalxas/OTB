## This file should be placed in the root directory of your project.
## Then modify the CMakeLists.txt file in the root directory of your
## project to incorporate the testing dashboard.
## # The following are required to uses Dart and the Cdash dashboard
##   enable_testing()
##   include(Dart)
set(CTEST_PROJECT_NAME "OTB")
set(CTEST_NIGHTLY_START_TIME "20:00:00 CEST")

set(CTEST_DROP_METHOD "https")
set(CTEST_DROP_SITE "dash.orfeo-toolbox.org")
set(CTEST_DROP_LOCATION "/submit.php?project=OTB")
set(CTEST_DROP_SITE_CDASH TRUE)
