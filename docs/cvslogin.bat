@echo off
set CVS_RSH=ssh
set CVSROOT=:ext:%1@sx3.dhs.org:/cvs

echo "Finished.  Please check your environment variables to make sure that"
echo "CVS_RSH and CVSROOT are set properly."
