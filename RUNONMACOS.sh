for f in ./Bannnjy.app/Contents/Frameworks/*
do
	t=$(basename $f)
	install_name_tool -change /opt/local/lib/$t @executable_path/../Frameworks/$t ./Bannnjy.app/Contents/MacOS/Bannnjy
	install_name_tool -change /opt/local/lib/$t @executable_path/../Frameworks/$t ./Bannnjy.app/Contents/Frameworks/libsfml-window.2.5.dylib
	install_name_tool -change /opt/local/lib/$t @executable_path/../Frameworks/$t ./Bannnjy.app/Contents/Frameworks/libsfml-graphics.2.5.dylib
	install_name_tool -change /opt/local/lib/$t @executable_path/../Frameworks/$t ./Bannnjy.app/Contents/Frameworks/libsfml-system.2.5.dylib
	echo "$t done"
done

install_name_tool -change /opt/local/lib/libpng16.16.dylib @executable_path/../Frameworks/libpng16.16.dylib ./Bannnjy.app/Contents/Frameworks/libfreetype.6.dylib
