Remove-Item -LiteralPath "Build" -Force -Recurse
Remove-Item -LiteralPath "Build.zip" -Force -Recurse

Copy-Item -Path Game -Destination Build -Recurse -Force -Container
Copy-Item Release\OrionEngine.exe -Destination Build

Compress-Archive -Path Build -DestinationPath Build.zip
