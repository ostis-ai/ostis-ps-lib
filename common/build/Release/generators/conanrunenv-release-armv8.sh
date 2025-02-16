script_folder="/Users/nikitazotov/Development/apps/ostis/ostis-ai/ostis-ps-lib/common/build/Release/generators"
echo "echo Restoring environment" > "$script_folder/deactivate_conanrunenv-release-armv8.sh"
for v in PATH LD_LIBRARY_PATH DYLD_LIBRARY_PATH
do
    is_defined="true"
    value=$(printenv $v) || is_defined="" || true
    if [ -n "$value" ] || [ -n "$is_defined" ]
    then
        echo export "$v='$value'" >> "$script_folder/deactivate_conanrunenv-release-armv8.sh"
    else
        echo unset $v >> "$script_folder/deactivate_conanrunenv-release-armv8.sh"
    fi
done


export PATH="/Users/nikitazotov/.conan2/p/b/sc-ma02cfe8ee8fbf6/p/bin:$PATH"
export LD_LIBRARY_PATH="/Users/nikitazotov/.conan2/p/b/sc-ma02cfe8ee8fbf6/p/lib:$LD_LIBRARY_PATH"
export DYLD_LIBRARY_PATH="/Users/nikitazotov/.conan2/p/b/sc-ma02cfe8ee8fbf6/p/lib:$DYLD_LIBRARY_PATH"