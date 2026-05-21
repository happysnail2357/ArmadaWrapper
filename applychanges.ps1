# applychanges.ps1
#
# Copies the latest build outputs into the Armada installation directory.
# Intended for quick deployment and testing during development.
#
# NOTE:
#   This script must be run with Administrator privileges.


# ========================
# Script parameters
# ========================

param (
    [switch]$Clean,
    [switch]$Release
)

$files = @("winmm.dll", "armadawrap.dll")

# This script should be run in the project root
$ProjectDir = "."

# Default Armada install directory
$TargetDir = Join-Path ${env:ProgramFiles(x86)} "Activision\Star Trek - Armada"


# ========================
# Function definitions
# ========================

function Copy-IfNewer {
    param (
        [Parameter(Mandatory=$true)]
        [string]$File
    )
    
    if ($Release) {
        $SourcePath = Join-Path -Path $ProjectDir -ChildPath "Release\"
    }
    else {
        $SourcePath = Join-Path -Path $ProjectDir -ChildPath "Debug\"
    }
    
    $SourcePath = Join-Path -Path $SourcePath -ChildPath $File
    $DestinationPath = Join-Path -Path $TargetDir -ChildPath $File

    if (-not (Test-Path $SourcePath)) {
        Write-Warning "Source file '$File' does not exist."
        return
    }

    $copyNeeded = $true

    if (Test-Path $DestinationPath) {
        $srcTime = (Get-Item $SourcePath).LastWriteTime
        $dstTime = (Get-Item $DestinationPath).LastWriteTime
        $copyNeeded = $srcTime -gt $dstTime
    }

    if ($copyNeeded -or $Release) {
        try {
            Copy-Item -Path $SourcePath -Destination $DestinationPath -Force -ErrorAction Stop
            
            $CopyMessage = "Copied: $File"
            
            if ($Release) {
                $CopyMessage += " (Release)"
            }
            
            Write-Host $CopyMessage
        }
        catch {
            Write-Error "Failed to copy ${File}: $($_.Exception.Message)"
        }
    } else {
        Write-Host "Skipped (unchanged): $File"
    }
}

function Delete-IfPresent {
    param (
        [Parameter(Mandatory=$true)]
        [string]$File
    )
    
    $FilePath = Join-Path -Path $TargetDir -ChildPath $File
    
    if (-not (Test-Path $FilePath)) {
        Write-Host "Not present: $File"
        return
    }
    
    try {
        Remove-Item -Path $FilePath
        Write-Host "Deleted: $File"
    }
    catch {
        Write-Error "Failed to remove ${File}: $($_.Exception.Message)"
    }
}


# ========================
# Main script logic
# ========================

if ($Clean) {
    foreach ($file in $files) {
        Delete-IfPresent $file
    }
}
else {
    foreach ($file in $files) {
        Copy-IfNewer $file
    }
}
