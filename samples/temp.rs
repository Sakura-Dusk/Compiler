fn main() {
    let string1 = r"\n";
    let string2 = r"\";
    let string3 = br##"Another one with #" and ##" inside."##;

    let table: [[i32; 3]; 2] = [
        [10, 20, 30],
        [40, 50, 60],
    ];
    let row: [i32; 2] = table[0];
}