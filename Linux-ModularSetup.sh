RootFolder="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
LocalFolder="$RootFolder/Local"
PackageFolder="$LocalFolder/Packages"
CmakeScriptFolder="$PackageFolder/Engine.CMake.Scripts"
EntryPointFolder="$PackageFolder/Engine.Main"

git config --global credential.helper cache

if [ ! -d "$LocalFolder" ]; then
	mkdir $LocalFolder
	cd $LocalFolder
	git init
	git remote add origin https://github.com/JKapostins/Engine.Linux.Build.Scripts.git
fi
cd $LocalFolder
git pull origin master

if [ ! -d "$PackageFolder" ]; then
        mkdir $PackageFolder
fi

if [ ! -d "$CmakeScriptFolder" ]; then
        mkdir $CmakeScriptFolder
        cd $CmakeScriptFolder
        git init
	git remote add origin https://github.com/JKapostins/Engine.CMake.Scripts.git
fi
cd $CmakeScriptFolder
git pull origin master

if [ ! -d "$EntryPointFolder" ]; then
        mkdir $EntryPointFolder
        cd $EntryPointFolder
        git init
	git remote add origin https://github.com/JKapostins/Engine.Main.git
fi
cd $EntryPointFolder
git pull origin master
