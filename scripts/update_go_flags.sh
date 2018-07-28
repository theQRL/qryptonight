echo $1
mv $1/goqryptonight/goqryptonight.go $1/goqryptonight/temp.go
sed '/#define intgo swig_intgo/i #cgo LDFLAGS: '$1'/goqryptonight/goqryptonight.so\n#cgo CXXFLAGS: -I'$1'/src -I'$1'/deps/xmr-stak\n' $1/goqryptonight/temp.go > $1/goqryptonight/goqryptonight.go
rm $1/goqryptonight/temp.go
