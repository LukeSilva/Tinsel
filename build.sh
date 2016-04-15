
echo "----Building tnl2c libraries----"

cd libs/C
mkdir -p h

cp tinsel_array.h h/

echo " - tnl2c.h"
cat templates/tnl2c_template > h/tnl2c.h



echo "#include \"String.h\"" >> h/tnl2c.h
echo "#include \"ConsoleIO.h\"
Console std_in;
Console std_out;" >> h/tnl2c.h

cd ../../

echo "----Building the stdlib for C----"

cstdlibfiles=("String" "IO" "FileIO" "ConsoleIO")
for i in "${cstdlibfiles[@]}"
do
	if [ -e "libs/C/$i.tnl" ]
	then
		echo " - $i"
		bash -e ctinsel.sh "libs/C/$i.tnl" "libs/C/h/$i.h"
	elif [ -e "libs/base/$i.tnl" ]
	then
		echo " - $i (base)"
		bash -e ctinsel.sh "libs/base/$i.tnl" "libs/C/h/$i.h"
	fi
done
