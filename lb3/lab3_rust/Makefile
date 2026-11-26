.PHONY: all check test lint bench coverage run clean
all:
	cargo build --release

check:
	cargo check

test:
	cargo test

lint:
	cargo clippy --all-targets -- -D warnings

bench:
	cargo run --release --bin benchmark -- 2000

coverage:
	cargo llvm-cov --html --ignore-filename-regex 'src/(main\.rs|bin/benchmark\.rs)$' --fail-under-lines 85

run:
	cargo run --release --bin dbms -- $(ARGS)

clean:
	cargo clean
