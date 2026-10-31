use std::process::{Command, Output};
use std::path::PathBuf;

type Err = Box<dyn std::error::Error>;
type Res<T> = std::result::Result<T, Err>;

fn main() -> Res<()> {
    let cur_dir: PathBuf = std::env::current_dir()?;
    let final_dir = cur_dir.join("bin");
    if !final_dir.exists() { println!("What the fuck"); }

    for i in 0..10 {
        let mut results: [f64; 100] = [0f64; 100];
        for (outer_index, _) in (0..100).enumerate() {            
            let cmd: Output = Command::new("./cdiscord").current_dir(&final_dir).output()?;
            if !cmd.status.success() { return Err("Failed to call the command".into()); }
            

            let out_str: String = String::from_utf8_lossy_owned(cmd.stdout);
            let mut num: f64 = 0.0;

            for elem in out_str.split(' ') {
                let parsed = elem.parse::<f64>();
                if parsed.is_ok() {
                    num = parsed?;
                }
            }

            results[outer_index] = num;
        }

        let avg: f64 = results.iter().sum::<f64>() / 100.0;
        results.sort_by(f64::total_cmp);

        let slowest = results.last().unwrap();
        let fastest = results.first().unwrap();

        println!("Average loop runtime: {avg} us | slowest run took {slowest} us | fastest took {fastest} us | in batch {i}");      
    }
    
    Ok(())
}



