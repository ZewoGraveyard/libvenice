TARGET = libvenice
LIB_NAME = venice
PKG_NAME = libvenice

all: $(TARGET)

$(TARGET): *.c
	 clang -c *.c
	 ar -rcs lib$(LIB_NAME).a *.o
	 rm *.o
	 mkdir -p $(TARGET)/DEBIAN
	 mkdir -p $(TARGET)/usr/local/lib
	 mkdir -p $(TARGET)/usr/local/include/$(TARGET)
	 cp *.h $(TARGET)/usr/local/include/$(TARGET)
	 cp lib$(LIB_NAME).a $(TARGET)/usr/local/lib/
	 touch $(TARGET)/DEBIAN/control
	 echo "Package: $(PKG_NAME)" >> $(TARGET)/DEBIAN/control
	 echo "Version: 1.0" >> $(TARGET)/DEBIAN/control
	 echo "Section: custom" >> $(TARGET)/DEBIAN/control
	 echo "Priority: optional" >> $(TARGET)/DEBIAN/control
	 echo "Architecture: all" >> $(TARGET)/DEBIAN/control
	 echo "Essential: no" >> $(TARGET)/DEBIAN/control
	 echo "Installed-Size: 1024" >> $(TARGET)/DEBIAN/control
	 echo "Maintainer: zewo.io" >> $(TARGET)/DEBIAN/control
	 echo "Description: $(TARGET)" >> $(TARGET)/DEBIAN/control
	 dpkg-deb --build $(TARGET)
	 rm -rf lib$(LIB_NAME).a
	 rm -rf $(TARGET)
