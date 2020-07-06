Get-ChildItem . -recurse -include *.fbx.meta | remove-item

Remove-Item -LiteralPath "Game/Library" -Force -Recurse