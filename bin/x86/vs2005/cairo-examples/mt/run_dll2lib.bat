pexports libcairo-2.dll > libcairo-2.def
call "%VS80COMNTOOLS%\vsvars32.bat"
lib /machine:i386 /def:libcairo-2.def