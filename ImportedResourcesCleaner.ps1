Get-ChildItem . -recurse -include *.meta | remove-item
Get-ChildItem . -recurse -include *.mesh | remove-item
Get-ChildItem . -recurse -include *.matol | remove-item
Get-ChildItem . -recurse -include *.sk | remove-item
Get-ChildItem . -recurse -include *.options | remove-item

Remove-Item -LiteralPath "Game/Library" -Force -Recurse