Remove-Item -LiteralPath "Engine" -Force -Recurse
Remove-Item -LiteralPath "Engine.zip" -Force -Recurse

Remove-Item -LiteralPath "TwinlightSouls" -Force -Recurse
Remove-Item -LiteralPath "TwinlightSouls.zip" -Force -Recurse.

Copy-Item -Path Game -Destination Engine -Recurse -Force -Container
Copy-Item Release\LittleOrionEngine.exe -Destination Engine

Compress-Archive -Path Engine -DestinationPath Engine.zip

Copy-Item -Path Game -Destination TwinlightSouls -Recurse -Force -Container
Copy-Item TwinlightSouls\TwinlightSouls.exe -Destination TwinlightSouls

Remove-Item -LiteralPath "TwinlightSouls\Assets" -Force -Recurse
Compress-Archive -Path TwinlightSouls -DestinationPath TwinlightSouls.zip