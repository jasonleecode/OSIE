# Unit Tests

## Framework 

Criterion: https://github.com/Snaipe/Criterion

## Criterion installation for Ubuntu (>=21.04)/Debian (>=11)

apt-get install libcriterion-dev

## Compile
```
make all
```

## Execute Tests
For `test_common` to execute:
```
./test_common
```

For `test_common` to execute and get a `tap` conform output execute:
```
./test_common --tap=test.tap
```