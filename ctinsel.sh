filename=$(basename "$1")
mainfunction=""
if [ ! -z "$3" ]
then
 # This is where we need to do function parsing to create help and main() functions for the entry point
 cmdargs=$( sed --posix -n -e "s/int $3.*(\(.*\))/\1/p" $1 )
 cmdargsarr=$(echo $cmdargs | sed -e "s/\s*,\s*/\n/g" | sed -e "s/ /__ctinsel__/g")
 mainfunction="int $3($cmdargs); int main(int argc,char** argv){ std_in.open(); std_out.open();"
 for x in $cmdargsarr
 do

  name=$(echo $x | sed --posix -e "s/.*__ctinsel__\(.*\)/\1/")
  type=$(echo $x | sed --posix -e "s/\(.*\)__ctinsel__.*/\1/")
  mainfunction="$mainfunction $type $name"
  if [ "$type" = "bool" ]
  then
   mainfunction="$mainfunction = false;"
	elif [ "$type" = "int" ]
	then
		mainfunction="$mainfunction = 0;"
  else
   mainfunction="$mainfunction;"
  fi
 
 done
 mainfunction="$mainfunction for (int i = 2; i < argc; i+=2){ "


 for x in $cmdargsarr
 do

  name=$(echo $x | sed --posix -e "s/.*__ctinsel__\(.*\)/\1/")
  type=$(echo $x | sed --posix -e "s/\(.*\)__ctinsel__.*/\1/")
  mainfunction="$mainfunction if(strcmp(\"--$name\",argv[i-1])==0) "
  if [ "$type" = "bool" ]
  then
   mainfunction="$mainfunction $name=strcmp(argv[i],\"true\")==0;"
  elif [ "$type" = "String" ]
  then
   mainfunction="$mainfunction $name=String(argv[i]);"
  elif [ "$type" = "int" ]
  then
   mainfunction="$mainfunction $name=std::stoi(argv[i]);"
  fi
 done
 mainfunction="$mainfunction } return $3("

 comma=false
 for x in $cmdargsarr
 do
  name=$(echo $x | sed --posix -e "s/.*__ctinsel__\(.*\)/\1/")
  if $comma
  then
   mainfunction="$mainfunction ,"
  fi
  mainfunction="$mainfunction $name"
  comma=true
 done 
 mainfunction="$mainfunction ); }" 
fi

echo "#pragma once
#include \"tnl2c.h\" 
$mainfunction" | cat - $1 | sed --posix -e "s/\([a-zA-Z0-9]*\) array/tinsel_array\<\1\>/g" -e "s/import \(.*\)/#include \\\"\1.h\\\"/g" -e "s/extends/\: public/g" -e "s/self\./this\-\>/g" -e "s/self\./this->/g" -e "s/String /__string__/g" -e "s/\([a-zA-Z]*\)(\([A-Z][a-zA-Z0-9]*\), \([A-Z][a-zA-Z0-9]*\) \(.*\))/\1(\2\&, \3& \4)/g" -e "s/\([a-zA-Z]*\)(\([A-Z][a-zA-Z0-9]*\) \(.*\))/\1(\2\& \3)/g" -e "s/__string__/String /g" > $2
