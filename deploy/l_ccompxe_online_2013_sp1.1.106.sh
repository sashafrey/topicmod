#!/bin/sh

# Copyright (c) 2012-2013 Intel Corporation. All rights reserved.
# This script installs Intel(R) Software Development Products.

IRC_URL="https://registrationcenter.intel.com"
PRODUCT_ID=l_ccompxe_2013_sp1.1.106
download_url=
irc_url=
wget_secure_protocol=auto

# strings section
bootstrap_error_prefix="ERROR:"
ja_bootstrap_error_prefix="エラー:"

bootstrap_prerequisites_checking_message="Before proceeding, the installer will check few prerequisites..."
ja_bootstrap_prerequisites_checking_message="続行する前に、インストーラーは必要条件をチェックします..."

bootstrap_prerequisites_checking_tool_message=" - Checking for"
ja_bootstrap_prerequisites_checking_tool_message=" - チェックしています:"

bootstrap_prerequisites_unable_to_find_tool_begin_message="Cannot find"
ja_bootstrap_prerequisites_unable_to_find_tool_begin_message="見つかりません"

bootstrap_prerequisites_unable_to_find_tool_end_message="Please, install it and restart the installation."
ja_bootstrap_prerequisites_unable_to_find_tool_end_message="見つからないツールをインストールしてから、再度インストールを実行してください。"

bootstrap_prerequisites_failed="[failed]"
ja_bootstrap_prerequisites_failed="[失敗]"

bootstrap_prerequisites_success="[success]"
ja_bootstrap_prerequisites_success="[成功]"

bootstrap_download_filename_error_message="Filename to download is not specified."
ja_bootstrap_download_filename_error_message="ダウンロードするファイル名が指定されていません。"

bootstrap_verify_checksum_filename_error_message="Filename to verify checksum is not specified."
ja_bootstrap_verify_checksum_filename_error_message="チェックサムを確認するファイル名が指定されていません。"

bootstrap_wget_protocol_message="Wget security protocol is:"
ja_bootstrap_wget_protocol_message="Wget セキュリティー・プロトコル:"

bootstrap_checking_irc_message="Checking Intel(R) Software Development Products Registration Center availability..."
ja_bootstrap_checking_irc_message="インテル(R) ソフトウェア開発製品レジストレーション・センターに接続しています..."

bootstrap_irc_is_not_reachable_error_1="Unable to connect to Intel(R) Software Development Products Registration Center"
bootstrap_irc_is_not_reachable_error_2="Some possible causes:"
bootstrap_irc_is_not_reachable_error_3="    1) Internet connection not found"
bootstrap_irc_is_not_reachable_error_4="        (Verify that you are connected to the internet.)"
bootstrap_irc_is_not_reachable_error_5="    2) Proxy settings are incorrect"
bootstrap_irc_is_not_reachable_error_6="        (If you are behind a proxy, please make sure that both of your http_proxy"
bootstrap_irc_is_not_reachable_error_7="         and https_proxy environment variables are set correctly.)"
bootstrap_irc_is_not_reachable_error_8="    3) Firewall is on. Please make sure the firewall settings leave ports"
bootstrap_irc_is_not_reachable_error_9="       80 and 443 opened."

ja_bootstrap_irc_is_not_reachable_error_1="インテル(R) ソフトウェア開発製品レジストレーション・センターに接続できません"
ja_bootstrap_irc_is_not_reachable_error_2="次の原因が考えられます。"
ja_bootstrap_irc_is_not_reachable_error_3="    1) インターネット接続が見つからない"
ja_bootstrap_irc_is_not_reachable_error_4="        (インターネットに接続しているかを確認してください。)"
ja_bootstrap_irc_is_not_reachable_error_5="    2) プロキシー設定が不正"
ja_bootstrap_irc_is_not_reachable_error_6="        (プロキシーを使用している場合は、http_proxy と https_proxy 環境変数が"
ja_bootstrap_irc_is_not_reachable_error_7="         適切に設定されていることを確認してください。)"
ja_bootstrap_irc_is_not_reachable_error_8="    3) ファイアウォールが有効 ファイアウォールの設定で、ポート番号 80 と 443 "
ja_bootstrap_irc_is_not_reachable_error_9="       が開いていることを確認してください。"

bootstrap_temporary_files_location_message="Temporary files will be downloaded to:"
ja_bootstrap_temporary_files_location_message="一時ファイルは次の場所にダウンロードされます:"

bootstrap_downloading_package_content_file_message="Downloading the package content file, this might take several minutes...     "
ja_bootstrap_downloading_package_content_file_message="パッケージ・コンテンツ・ファイルをダウンロードしています。しばらくお待ちください...     "

bootstrap_failed_to_download_package_content_file_message="Cannot download package content file."
ja_bootstrap_failed_to_download_package_content_file_message="パッケージ・コンテンツ・ファイルをダウンロードできません。"

bootstrap_downloading_installer_message="Downloading the installer, this might take several minutes...     "
ja_bootstrap_downloading_installer_message="インストーラーをダウンロードしています。しばらくお待ちください...     "

bootstrap_failed_to_download_installer_message="Cannot download the installer."
ja_bootstrap_failed_to_download_installer_message="インストーラーをダウンロードできません。"

bootstrap_installer_checksum_verification_error_message="Installer checksum verification failed."
ja_bootstrap_installer_checksum_verification_error_message="インストーラーのチェックサム検証に失敗しました。"

bootstrap_installer_redownload_message="The installer will be downloaded again."
ja_bootstrap_installer_redownload_message="インストーラーは再度ダウンロードされます。"

bootstrap_failed_to_extract_installer="Failed to extract installer."
ja_bootstrap_failed_to_extract_installer="インストーラーの展開に失敗しました。"

bootstrap_welcome_message="Welcome to the online installer for"
ja_bootstrap_welcome_message=""

string_variables_names="bootstrap_error_prefix bootstrap_prerequisites_checking_message bootstrap_prerequisites_checking_tool_message bootstrap_prerequisites_unable_to_find_tool_begin_message bootstrap_prerequisites_unable_to_find_tool_end_message bootstrap_prerequisites_failed bootstrap_prerequisites_success bootstrap_download_filename_error_message bootstrap_verify_checksum_filename_error_message bootstrap_wget_protocol_message bootstrap_checking_irc_message bootstrap_irc_is_not_reachable_error_1 bootstrap_irc_is_not_reachable_error_2 bootstrap_irc_is_not_reachable_error_3 bootstrap_irc_is_not_reachable_error_4 bootstrap_irc_is_not_reachable_error_5 bootstrap_irc_is_not_reachable_error_6 bootstrap_irc_is_not_reachable_error_7 bootstrap_irc_is_not_reachable_error_8 bootstrap_irc_is_not_reachable_error_9 bootstrap_temporary_files_location_message bootstrap_downloading_package_content_file_message bootstrap_failed_to_download_package_content_file_message bootstrap_downloading_installer_message bootstrap_failed_to_download_installer_message bootstrap_installer_checksum_verification_error_message bootstrap_installer_redownload_message bootstrap_failed_to_extract_installer bootstrap_welcome_message"

DIE ()
{
    if [ "$1" != "" ]; then
        echo "${bootstrap_error_prefix} $1"
    fi

    exit 1
}

check_tools ()
{
    local tools_to_check="wget awk tar cksum"
    echo "${bootstrap_prerequisites_checking_message}"
    for tool in $tools_to_check ; do
        echo -n "${bootstrap_prerequisites_checking_tool_message} '$tool'..."
        which $tool 2>/dev/null 1>&2
        if [ $? -ne 0 ]; then
            echo " ${bootstrap_prerequisites_failed}"
            DIE "${bootstrap_prerequisites_unable_to_find_tool_begin_message} '$tool'. ${bootstrap_prerequisites_unable_to_find_tool_end_message}"
        else
            echo " ${bootstrap_prerequisites_success}"
        fi
    done
}

touch_space ()
{

    local dir_to_check=$1
    local dir_end_path='hags7823782318#@123kjhknmnzxmnz'
    local err=0

    if [ -d "$dir_to_check" ] ; then
        if [ -w "$dir_to_check" ] ; then
            mkdir "$dir_to_check/$dir_end_path" > /dev/null 2>&1
            err=$?
            if [ "$err" = "0" ] ; then
                rmdir "$dir_to_check/$dir_end_path" > /dev/null 2>&1
            fi
        else
            err=1
        fi
    else
        touch_space "`dirname $dir_to_check`" "$dir_end_path"
        err=$?
    fi

    return $err
}

parse_cmd_parameters ()
{
    while [ $# -gt 0 ] ; do
    case "$1" in
        --silent|-s)
            # silent install
            if [ -z "$2" ]; then
                echo "Error: Please provide silent configuration file."
                exit 1
            fi
            if [ ! -e "$2" ]; then
                echo "Error: specified silent configuration file does not exist."
                exit 1
            fi
            exe=`basename $2 2>/dev/null`
            dir=`dirname $2 2>/dev/null`
            if echo $dir | grep -q -s ^/ || echo $dir | grep -q -s ^~ ; then
                # absolute path
                silent_params="--silent $dir/$exe"
                silent_cfg="$dir/$exe"
            else
                # relative path
                silent_params="--silent $runningdir/$dir/$exe"
                silent_cfg="$runningdir/$dir/$exe"
            fi
            if [ ! -f "$silent_cfg" ]; then
                echo "Error: \"$silent_cfg\" doesn't look like a proper silent configuration file."
                echo "Please make sure that this file exists and run installation again."
                exit 1
            fi
            skip_uid_check="yes"
            shift
            ;;
        --help|-h)
            echo "This script installs Intel(R) Software Development Products."
            echo ""
            echo "Usage: install.sh [options]"
            echo ""
            echo "    -h, --help                         print this message"
            echo "    -v, --version                      print version information"
            echo "    -t, --tmp-dir [FOLDER]             set custom temporary folder"
            echo "    -D, --download-tmp-dir [FOLDER]    set custom download folder"
            echo "    -o, --download-only                download package only"
            echo "    -s, --silent [FILE]                run install silently, with settings in the"
            echo "                                       configuration file"
            echo "    -d, --duplicate [FILE]             run install interactively, record the user"
            echo "                                       input into the configuration file"
            echo "    --cli-mode                         run install in command-line mode"
            echo "    --gui-mode                         run install in graphical mode"
            echo "    --user-mode                        run install with current user privileges"

            echo "Copyright (C) 2012-2013 Intel Corporation. All rights reserved."
            exit 0
            ;;
        --version|-v)
            # show version info
            echo "Product name: Intel(R) C++ Composer XE 2013 SP1"
            echo "Package id: l_ccompxe_2013_sp1.1.106"
            exit 0
            ;;
        --duplicate|-d)
            # duplicate install
            if [ -z "$2" ]; then
                echo "Error: Please provide silent configuration file."
                exit 1
            fi
            exe=`basename $2 2>/dev/null`
            dir=`dirname $2 2>/dev/null`
            if echo $dir | grep -q -s ^/ || echo $dir | grep -q -s ^~ ; then
                # absolute path
                duplicate_params="--duplicate $dir/$exe"
                duplicate_cfg="$dir/$exe"
            else
                # relative path
                duplicate_params="--duplicate $runningdir/$dir/$exe"
                duplicate_cfg="$runningdir/$dir/$exe"
            fi
            shift
            ;;
        --download-url)
            if [ -z "$2" ]; then
                echo "Error: download url must be specified."
                exit 1
            fi
            download_url="$2"
            echo "$download_url" | grep "\/$" > /dev/null 2>&1
            if [ "$?" = "1" ]; then
                download_url="${download_url}/"
            fi

            shift
            ;;
         --irc-url)
            if [ -z "$2" ]; then
                echo "Error: IRC url must be specified."
                exit 1
            fi
            irc_url="$2"

            shift
            ;;
        --download-tmp-dir|-D)
            if [ -z "$2" ]; then
                echo "Error: Please provide download temporal folder."
                exit 1
            fi
            download_tmp="$2"
            if [ ! -d "$download_tmp" ]; then
                echo "Error: $download_tmp doesn't look like a proper folder."
                echo "Please make sure that this folder exists and run installation again."
                exit 1
            fi

            shift
            ;;
 
        --tmp-dir|-t)
            if [ -z "$2" ]; then
                echo "Error: Please provide temporal folder."
                exit 1
            fi
            user_tmp="$2"
            if [ ! -d "$user_tmp" ]; then
                echo "Error: $user_tmp doesn't look like a proper folder."
                echo "Please make sure that this folder exists and run installation again."
                exit 1
            fi

            shift
            ;;
    
        --cli-mode)
            # start CLI installer
            start_cli_install="yes"
            params="$params --cli-mode"
            ;;

        --gui-mode)
            # start GUI installer (default behavior)
            ;;

        --download-only|-o)
            params="$params --download-only"
            ;;
        *)
            params="$params $1"
            #check for LANG option
            is_lang=$(echo $1 | grep "LANG")
            if [ ! -z "$is_lang" ]; then
                user_lang=$(echo $1 | cut -d= -f2)
            fi
            ;;
        esac
        shift
    done
}

check_runningdir()
{
    if [ -n "$(echo "$fullpath" | grep " ")" ] ; then
        echo "Error: Incorrect path to installation script. Installation can not be started"
        echo "if the path contains space symbols."
        echo ""
        echo "Quitting! Press \"Enter\" to terminate install."
        read DONTCARE
        exit 1
    fi

    if [ -n "$(echo "$fullpath" | egrep -e ':' -e '~' -e '&' -e '%' -e '#' -e '@' -e '\[' -e '\]' -e '\$' -e '=' -e '\)' -e '\(' -e '\*')" ] ; then
        echo "Error: Incorrect path to installation script. Installation can not be started"
        echo "if the path contains ':, ~, @, #, %, &, [, ], $, =, ), (, *' symbols."
        echo ""
        echo "Quitting! Press \"Enter\" to terminate install."
        read DONTCARE
        exit 1
    fi
}

download_file ()
{
    local file_name=$1
    if [ -z "$file_name" ]; then
        DIE "${bootstrap_download_filename_error_message}"
    fi
    wget --secure-protocol=$wget_secure_protocol --progress=dot --continue --directory-prefix=$download_tmp -O $file_name "${download_url}${file_name}" 2>&1 | grep --line-buffered "%" | sed -u -e "s/\.//g" | sed -u -e "s/,//g" | awk -W interactive '{printf("\b\b\b\b%4s", $2)}' 2>>$log_file
    if [ $? -ne 0 ]; then
        return 0
    fi
    if [ -f $download_tmp/$file_name ]; then
        local file_size=""
        file_size=$(stat -c %s $file_name)
        if [ "$file_size" != "0" ]; then
            return 0
        fi
    fi
    return 1
}

verify_checksum ()
{
    local file_name=$1
    if [ -z "$file_name" ]; then
        DIE "${bootstrap_verify_checksum_filename_error_message}"
    fi
    local cksum=$(cat product.xml | grep $file_name | sed 's/.*cksum="\([0-9]*\)".*/\1/' 2>/dev/null)
    local real_cksum=$(cksum $file_name | cut -d ' ' -f1 2>/dev/null)
    if [ "$cksum" != "$real_cksum" ]; then
        return 1
    else
        return 0
    fi
}

check_irc_availability ()
{
    wget_secure_protocol="SSLv3"
    return 0
}

################################################################################
# Script Entry Point
################################################################################

if [ "x$LANG" = "xja_JP.UTF-8" ] || [ "x$LANG" = "xja_JP.utf8" ] ; then
    for var_name in $string_variables_names ; do
        eval $var_name=\${ja_$var_name}
    done
fi

# detect bash script source execution
if [ -z "$(echo "$0" | grep 'online')" ]; then
    echo "Script is running sourced..."
    echo "ERROR: This script should be called directly."
    
    exit 1
fi

start_cli_install=no
thisexec=`basename $0`
thisdir=`dirname $0`
runningdir=`pwd`

if echo $thisdir | grep -q -s ^/ || echo $thisdir | grep -q -s ^~ ; then
# absolute path
   fullpath="$thisdir"
else
# relative path 
   fullpath="$runningdir/$thisdir"
fi
check_runningdir

parse_cmd_parameters $@
if [ -z "$user_tmp" ]; then
    if [ -z "$TMPDIR" ]; then
        user_tmp="/tmp"
    else
        if [ -d "$TMPDIR" ]; then
            user_tmp=$TMPDIR
        else
            user_tmp="/tmp"
        fi
    fi
fi
if [ -z "$download_tmp" ]; then
    download_tmp=$user_tmp
fi

echo "${bootstrap_welcome_message} Intel(R) C++ Composer XE 2013 SP1."
echo "--------------------------------------------------------------------------------"
    
check_tools

[ -z "$HOSTNAME" ] && HOSTNAME=$(hostname);
log_file=intel.bootstrap.$USER.${HOSTNAME}_$(date +%m.%d.%H.%M.%S.%Y).log

touch_space $user_tmp
check=$?
if [ "$check" != "0" ]; then
    echo "Error: No write permissions to \"$user_tmp\" temporary folder."
    echo "Please fix the permissions or provide another temporary folder."
    echo "To provide another temporary folder please run installation"
    echo "with \"--tmp-dir [FOLDER]\" parameter."
    exit 1
fi

touch_space $download_tmp
check=$?
if [ "$check" != "0" ]; then
    echo "Error: No write permissions to \"$download_tmp\" temporary download folder."
    echo "Please fix the permissions or provide another temporary download folder."
    echo "To provide another temporary folder please run installation"
    echo "with \"--download-tmp-dir [FOLDER]\" parameter."
    exit 1
fi

log_file="$user_tmp/$log_file"
download_tmp=${download_tmp}/${USER}/${PRODUCT_ID}
mkdir -p $download_tmp

CDIR=`pwd`
cd $download_tmp

if [ "$download_url" = "" ]; then
    if [ "$irc_url" = "" ]; then
        irc_url=$IRC_URL
    fi
    download_url="${irc_url}/regcenter/getonlinepackagerooturl.aspx?id=${PRODUCT_ID}&file="

    echo "--------------------------------------------------------------------------------"
    echo -n "${bootstrap_checking_irc_message}"
    check_irc_availability
    if [ "$?" != "0" ]; then
        echo " ${bootstrap_prerequisites_failed}"
        echo "${bootstrap_irc_is_not_reachable_error_1}"
        echo ""
        echo "${bootstrap_irc_is_not_reachable_error_2}"
        echo ""
        echo "${bootstrap_irc_is_not_reachable_error_3}"
        echo "${bootstrap_irc_is_not_reachable_error_4}"
        echo ""
        echo "${bootstrap_irc_is_not_reachable_error_5}"
        echo "${bootstrap_irc_is_not_reachable_error_6}"
        echo "${bootstrap_irc_is_not_reachable_error_7}"
        echo ""
        echo "${bootstrap_irc_is_not_reachable_error_8}"
        echo "${bootstrap_irc_is_not_reachable_error_9}"
        exit 1
    else
        echo " ${bootstrap_prerequisites_success}"
    fi
fi

system_cpu=`uname -m`
if [ "$system_cpu" = "ia64" ]; then
    my_arch="ia64"
elif [ "$system_cpu" = "x86_64" ]; then
    my_arch="intel64" 
else
    my_arch="ia32"
fi
pset_archive_name="pset_${my_arch}.tgz"
echo "--------------------------------------------------------------------------------"
echo "${bootstrap_temporary_files_location_message}$download_tmp"
echo -n "${bootstrap_downloading_package_content_file_message}"
rm -rf product.xml 2>/dev/null
download_file "product.xml"
if [ "$?" != "0" ]; then
    echo " ${bootstrap_prerequisites_failed}"
    echo "${bootstrap_error_prefix} ${bootstrap_failed_to_download_package_content_file_message}"
    exit 1
else
    echo " ${bootstrap_prerequisites_success}"
fi

echo "--------------------------------------------------------------------------------"
echo -n "Downloading the installer, this might take several minutes...     "
download_file "$pset_archive_name"
if [ "$?" != "0" ]; then
    echo " ${bootstrap_prerequisites_failed}"
    echo "${bootstrap_error_prefix} ${bootstrap_failed_to_download_installer_message}"
    exit 1
else
    echo " ${bootstrap_prerequisites_success}"
fi

verify_checksum "$pset_archive_name"
if [ "$?" != "0" ]; then
    echo ""
    echo "${bootstrap_error_prefix} ${bootstrap_installer_checksum_verification_error_message}"
    echo -n "${bootstrap_installer_redownload_message}     "
    rm -rf $pset_archive_name 2>/dev/null

    download_file "$pset_archive_name"
    if [ "$?" != "0" ]; then
        echo " ${bootstrap_prerequisites_failed}"
        echo "${bootstrap_error_prefix} ${bootstrap_failed_to_download_installer_message}"
        exit 1
    else
        verify_checksum "$pset_archive_name"
        if [ "$?" != "0" ]; then
            echo ""
            echo "${bootstrap_error_prefix} ${bootstrap_installer_checksum_verification_error_message}"
            exit 1
        fi
        echo " ${bootstrap_prerequisites_success}"
    fi
fi

tar xzf "$pset_archive_name" 2>/dev/null
if [ "$?" != "0" ]; then
    echo "${bootstrap_error_prefix} ${bootstrap_failed_to_extract_installer}"
    exit 1
fi

if [ "$start_cli_install" = "yes" ]; then
    ./install.sh --download-url "$download_url" --tmp-dir $user_tmp --wget-secure-protocol $wget_secure_protocol $silent_params $duplicate_params $params 
else
    ./install_GUI.sh --download-url "$download_url" --tmp-dir $user_tmp --wget-secure-protocol $wget_secure_protocol $silent_params $duplicate_params $params 
fi
exit_code=$?
cd $CDIR
exit $exit_code
