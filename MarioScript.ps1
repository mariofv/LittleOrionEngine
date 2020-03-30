Get-ChildItem . -recurse -include *.meta | remove-item
Remove-Item -LiteralPath "Game/Library" -Force -Recurse 