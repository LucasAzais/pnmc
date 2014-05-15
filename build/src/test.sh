echo put args such as --tina samples/tina/filepath

./pnmc $1 $2
#java -jar drawer.jar order.txt render/$(basename "$2")_natural.png

./pnmc $1 --force1 $2 #--order-show
#java -jar drawer.jar order.txt render/$(basename "$2")_force.png

./pnmc $1 --force2 $2 #--order-show
#java -jar drawer.jar order.txt render/$(basename "$2")_force_enhanced.png

./pnmc $1 --force3 $2 #--order-show
#java -jar drawer.jar order.txt render/$(basename "$2")_force_enhanced.png

./pnmc $1 --force4 $2 #--order-show
#java -jar drawer.jar order.txt render/$(basename "$2")_force_enhanced.png