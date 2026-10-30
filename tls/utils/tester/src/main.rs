use std::borrow::Cow;
use std::process::{Command, Output};
use std::path::PathBuf;
use std::time::Instant;

type Err = Box<dyn std::error::Error>;
type Res<T> = std::result::Result<T, Err>;

fn main() -> Res<()> {
    let cur_dir: PathBuf = std::env::current_dir()?;
    let final_dir = cur_dir.join("bin");
    if !final_dir.exists() { println!("What the fuck"); }

    for i in 0..10 {
        let mut results: [u128; 100] = [0; 100];
        for (outer_index, _) in (0..100).enumerate() {
            let now: Instant = Instant::now();
            
            let cmd: Output = Command::new("./cdiscord").current_dir(&final_dir).output()?;
            if !cmd.status.success() { return Err("Failed to call the command".into()); }
            
            let end: Instant = Instant::now();
            results[outer_index] = (end - now).as_nanos();
        }

        let avg: f64 = results.iter().map(|i| *i as f64).sum::<f64>() / 100.0;
        let slowest: u128 = *results.iter().max().unwrap();
        let fastest: u128 = *results.iter().min().unwrap();

        println!("Average loop runtime: {avg} ns | slowest run took {slowest} ns | fastest took {fastest} ns | in batch {i}");      
    }
    
    Ok(())
}



