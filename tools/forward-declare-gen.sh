#!/bin/sh
###########################################################################
#    Lightspark, a free flash player implementation
#
#    Copyright (C) 2024  mr b0nk 500 (b0nk@b0nk.xyz)
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
###########################################################################

make_excludes() {
	ret="(";
	while [ $# -gt 0 ]; do
		ret="$ret -path $1";
		if [ $# -gt 1 ]; then
			ret="$ret -o";
		fi
		shift;
	done
	echo "$ret )";
	return 0;
}

make_pretty_list() {
	ret="$1";
	shift;
	while [ $# -gt 0 ]; do
		if [ $# -gt 1 ]; then
			ret="$ret, $1";
		else
			ret="$ret, and $1";
		fi
		shift;
	done
	echo "$ret";
	return 0;
}

make_author_list() {
	while [ $# -gt 0 ]; do
		author="$1";
		email="$2";
		shift 2;
		[ "$1" = "-s" ] && (start_year=$2; shift 2;)
		[ "$1" = "-e" ] && (end_year=$2; shift 2;)
		start_year=$(date +%Y);
		end_year=$(date +%Y);

		if [ $start_year -eq $end_year ]; then
			year_str="$start_year";
		else
			year_str="$start_year-$end_year";
		fi
		echo "    Copyright (C) $year_str  $author ($email)"
	done
}


if [ $# -gt 0 ]; then
	_done=0;
	while [ $# -gt 0 ]; do
		case "$1" in
			"-q") quiet=1; shift; ;;
			"-h") echo "usage: $0 [-q -h] [<author-name> <email-address> [-s start-year] [-e end-year]...]"; exit 0; ;;
			*) break; ;;
		esac
	done
	author_list=$(make_author_list "$@");
fi

[ -n "$author_list" ] && read -d '' copyright << EOF
/**************************************************************************
    Lightspark, a free flash player implementation

$author_list

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**************************************************************************/
EOF

forward_dir="forwards";
read -d '' exclude_list << EOF
$forward_dir
interfaces
3rdparty
plugin
plugin_ppapi
allclasses.h
compat.h
errorconstants.h
exceptions.h
logger.h
memory_support.h
platforms/fastpaths.h
smartrefs.h
EOF

typedef_regex="typedef\h+.*;"
type_decl_regex="(^template(\h*)?<.*>$\n?)?^(class|struct|union)(\h*)(?!.*<.*>).*[^;\n]$"

excludes=$(make_excludes $exclude_list)
header_files=$($(dirname "$0")/get-forward-headers.sh $exclude_list);
base_dir=$(dirname "$0")/..
cd $base_dir/src

for dir in $(find * -type d -path "*/.*" -prune -o $excludes -prune -o -type d -print); do
	[ ! -d $forward_dir/$dir ] && mkdir -p $forward_dir/$dir;
done

if [ -n "$copyright" ]; then
	header="$copyright\n\n";
fi
header="$header/* This file was generated by forward-declare-gen.sh. - DO NOT EDIT */\n";

make_file() {
	file="$1";
	output_file=$forward_dir/$file;
	interface_file=interfaces/$file;
	def_name="FORWARDS_$(echo "$file" | tr '/.[:lower:]' '__[:upper:]')";
	if [ ! -e $interface_file ]; then
		files=$file;
	else
		files="$file $interface_file";
	fi

	types="`pcregrep -Mh "$type_decl_regex" $files | sed '/template/!s/\s*DLL_PUBLIC//;/template/!s/\(\/\*.*\*\/\|\s*:\s*\(public\|private\).*[^;]\|\s{\|\s*$\)/;/'`";
	if [ -z "$types" ]; then
		rm -f $output_file;
		return;
	fi
	if [ ! $quiet ]; then
		pretty_files=$(make_pretty_list $files);
		echo "Creating forward declarations for $pretty_files in $forward_dir";
	fi

	echo -e "$header" > $output_file;
	echo -e "#ifndef $def_name" >> $output_file;
	echo -e "#define $def_name 1\n" >> $output_file;
	echo -e "namespace lightspark" >> $output_file;
	echo -e "{\n" >> $output_file;
	echo -e "/* forward declarations */" >> $output_file;
	echo -e "${types}" >> $output_file;
	echo -e "\n};" >> $output_file;
	echo -e "#endif /* $def_name */" >> $output_file;
}

for file in $header_files; do
	make_file $file &
done
wait;

touch $forward_dir/forwards.h;
