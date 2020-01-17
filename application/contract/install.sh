cd out
find . -name \*.py -exec cp {} ../../ \;
find . -name \*.cc -exec cp {} ../../castalia-m3wsn/src/node/communication/routing/SDN/ \;
find . -name \*.h -exec cp {} ../../castalia-m3wsn/src/node/communication/routing/SDN/ \;
