all:  wx.c wmr xml decode state
	c++ -o wx wx.c wmr xml decode sdstate
xml: xml_encode.cpp
	c++ -c xml_encode.cpp -o xml
decode: decode.cpp
	c++  -c decode.cpp -o decode
state:	sdstate.c
	c++ -c sdstate.c -o sdstate
wmr:	wmr.cpp
	c++ -c wmr.cpp -o wmr
clean: 
	rm sdstate decode xml wmr wx
deploy:
	cp wx ~/bin/