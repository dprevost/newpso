#!/bin/sh

java_src=$java_src"org.photon.BaseQueue "
java_src=$java_src"org.photon.DefinitionType "
java_src=$java_src"org.photon.FieldDefODBC "
java_src=$java_src"org.photon.FieldType "
java_src=$java_src"org.photon.FolderEntry "
java_src=$java_src"org.photon.Folder "
java_src=$java_src"org.photon.Info "
java_src=$java_src"org.photon.KeyDefinition "
java_src=$java_src"org.photon.KeyODBC "
java_src=$java_src"org.photon.KeyType "
java_src=$java_src"org.photon.ObjectDefinition "
java_src=$java_src"org.photon.ObjectStatus "
java_src=$java_src"org.photon.ObjectType "
java_src=$java_src"org.photon.PhotonErrors "
java_src=$java_src"org.photon.PhotonException "
java_src=$java_src"org.photon.Photon "
java_src=$java_src"org.photon.Queue "
java_src=$java_src"org.photon.Session "
java_src=$java_src"org.photon.TuppleQueue "

#java_src=$java_src"org.photon.ArrayListSerializer "
#java_src=$java_src"org.photon.JavaSerializer "
#java_src=$java_src"org.photon.PSOSerialize "

#javah -classpath ../../build/default/src/Java/java_src -d include org.photon.Queue


for src in $java_src ; do
  javah -classpath ../../build/default/src/Java/java_src -d include $src
   if [ "$?" != 0 ] ; then
      echo "Problems with : $src "
  fi
done


