@echo off

call:remove_from_path path Python27

goto:eof

:remove_from_path
  setlocal enabledelayedexpansion

  set "_listname=%~1"
  set "_substr=%~2"
  set "_list=!%_listname%!"
  for %%a in ("%_list:;=";"%") do (
    set "thing=%%~a"
    if "x!thing:%_substr%=!"=="x!thing!" (
      set "newpath=!newpath!;!thing!"
    )
  )
  (endlocal
    set "%~1=%newpath%"
  )
goto:eof
