echo put args such as --tina samples/tina/filepath

./pnmc $1 $2
./pnmc $1 $2 --force
./pnmc $1 $2 --force2
./pnmc $1 $2 --order_edges
./pnmc $1 $2 --order_edges --order_pre_post
./pnmc $1 $2 --order_edges_reversed
./pnmc $1 $2 --order_edges_reversed --order_pre_post
./pnmc $1 $2 --order_pre_post
./pnmc $1 $2 --force3

notify-send "$2 done !"