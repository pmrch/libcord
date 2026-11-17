$Dirs = @("bin", "build", "asm", "lib")

foreach ($dir in $Dirs) {
    if (Test-Path $dir) {
        Remove-Item "$dir\*" -Force -Recurse -Confirm:$false
    }
}