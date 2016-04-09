types=("u8" "s8" "u16" "s16" "u32" "s32" "int" "bool")

echo "----Building tnl2c libraries----"

cd libs/C
mkdir -p h
for i in "${types[@]}"
do
echo " - $i""_array.h"
sed -e "s/_type/$i/g" < templates/array_template > h/"$i"_array.h
done

echo " - tnl2c.h"
cat templates/tnl2c_template > h/tnl2c.h
for i in "${types[@]}"
do
echo "#include \"$i""_array.h\"" >> h/tnl2c.h
done
echo "#include \"String.h\"" >> h/tnl2c.h
echo "#include \"ConsoleIO.h\"
Console std_in;
Console std_out;" >> h/tnl2c.h


echo "----Building the String array type----"
sed -e "s/_type/String/g" < templates/array_template > h/String_array.h
echo "#include \"String_array.h\"" >> h/tnl2c.h
echo " - String_array.h"

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
