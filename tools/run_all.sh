#!/bin/bash

rm -rf result.out
for file_name in `ls ./in/*.txt`
do
../src/a.out < $file_name > ../out/newest.out
cargo run --release --bin vis $file_name ../out/newest.out >> result.out 
done

ruby calc.rb