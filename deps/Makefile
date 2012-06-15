
JS_VER = js-1.60
LUA_VER = lua-5.1.2
NSPR_VER = nspr-4.6.6

preparedeps:
	if [ ! -d js ]; then make extract; fi

clean:
	rm -rf js lua moztools nsprpub $(LUA_VER) $(NSPR_VER)

extract: clean download
	tar xvfz $(JS_VER).tar.gz
	tar xvfz $(NSPR_VER).tar.gz
	mv $(NSPR_VER)/mozilla/nsprpub .
	tar xvfz $(LUA_VER).tar.gz
	mv $(LUA_VER) lua
	unzip moztools-static.zip

cleanall:
	rm -rf *.tar.gz *.zip

download:
	wget -nc http://ftp.mozilla.org/pub/mozilla.org/mozilla/libraries/win32/moztools-static.zip
	wget -nc http://ftp.mozilla.org/pub/mozilla.org/js/js-1.60.tar.gz
	wget -nc http://ftp.mozilla.org/pub/mozilla.org/nspr/releases/v4.6.6/src/nspr-4.6.6.tar.gz
	wget -nc http://www.lua.org/ftp/lua-5.1.2.tar.gz
