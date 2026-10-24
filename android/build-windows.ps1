[CmdletBinding()]
param(
    [ValidateSet('GameNoAssetsDebug', 'GameNoAssetsRelease', 'GameFullDebug', 'GameFullRelease')]
    [string] $Variant = 'GameFullDebug',

    [ValidateSet('Legacy32', 'Arm64')]
    [string] $Architecture = 'Legacy32'
)

$ErrorActionPreference = 'Stop'

$repositoryPath = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$buildRepositoryPath = $repositoryPath
$temporaryDrive = $null
$targetAbi = if ($Architecture -eq 'Arm64') { 'arm64-v8a' } else { 'armeabi-v7a' }

if (-not $env:JAVA_HOME) {
    $javaCandidates = @(
        (Join-Path $env:LOCALAPPDATA 'Programs\Microsoft\jdk-17.0.10.7-hotspot'),
        (Join-Path $env:ProgramFiles 'Android\Android Studio\jbr')
    )
    $detectedJava = $javaCandidates | Where-Object { Test-Path (Join-Path $_ 'bin\java.exe') } | Select-Object -First 1
    if ($detectedJava) {
        $env:JAVA_HOME = $detectedJava
    }
}

if (-not $env:JAVA_HOME -or -not (Test-Path (Join-Path $env:JAVA_HOME 'bin\java.exe'))) {
    throw 'JDK 17 or newer was not found. Set JAVA_HOME before running this script.'
}

if (-not $env:ANDROID_HOME) {
    $defaultAndroidSdk = Join-Path $env:LOCALAPPDATA 'Android\Sdk'
    if (Test-Path $defaultAndroidSdk) {
        $env:ANDROID_HOME = $defaultAndroidSdk
    }
}

if (-not $env:ANDROID_HOME -or -not (Test-Path $env:ANDROID_HOME)) {
    throw 'The Android SDK was not found. Set ANDROID_HOME before running this script.'
}
$env:ANDROID_SDK_ROOT = $env:ANDROID_HOME

try {
    if ($repositoryPath -match '\s') {
        foreach ($letter in @('R', 'Q', 'P', 'O', 'N', 'M', 'L', 'K', 'J', 'I', 'H', 'G')) {
            if (-not (Test-Path "${letter}:\")) {
                & subst "${letter}:" $repositoryPath
                if ($LASTEXITCODE -ne 0) {
                    throw "Could not map ${letter}: to the repository."
                }
                $temporaryDrive = $letter
                $buildRepositoryPath = "${letter}:\"
                break
            }
        }

        if (-not $temporaryDrive) {
            throw 'No free temporary drive letter is available for the Android NDK build.'
        }
    }

    Push-Location (Join-Path $buildRepositoryPath 'android')
    try {
        & .\gradlew.bat "assemble$Variant" "-PtargetAbi=$targetAbi"
        if ($LASTEXITCODE -ne 0) {
            throw "Gradle failed with exit code $LASTEXITCODE."
        }
    }
    finally {
        Pop-Location
    }
}
finally {
    if ($temporaryDrive) {
        & subst "${temporaryDrive}:" /d
    }
}
