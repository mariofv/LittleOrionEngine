Remove-Item -LiteralPath "LittleOrionEngine" -Force -Recurse
Remove-Item -LiteralPath "LittleOrionEngine.zip" -Force -Recurse

Remove-Item -LiteralPath "TwinlightSouls" -Force -Recurse
Remove-Item -LiteralPath "TwinlightSouls.zip" -Force -Recurse.

Copy-Item -Path Game -Destination Engine -Recurse -Force -Container
Copy-Item Release\LittleOrionEngine.exe -Destination LittleOrionEngine

Compress-Archive -Path LittleOrionEngine -DestinationPath LittleOrionEngine.zip

Copy-Item -Path Game -Destination TwinlightSouls -Recurse -Force -Container
Copy-Item TwinlightSouls\TwinlightSouls.exe -Destination TwinlightSouls

Remove-Item -LiteralPath "TwinlightSouls\Assets" -Force -Recurse
Remove-Item -LiteralPath "TwinlightSouls\GamePlaySystemDebu_.dll" -Force -Recurse
Remove-Item -LiteralPath "TwinlightSouls\GamePlaySystemReleas_.dll" -Force -Recurse
Compress-Archive -Path TwinlightSouls -DestinationPath TwinlightSouls.zip