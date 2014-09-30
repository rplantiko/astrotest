// Wiederverwendare Funktionen in der Automation (cscript //nologo //e:jscript)

// Einen Befehl ausführen (mittels "exec)
// stdin, path und environment params können übergeben werden
// Rückgabe: stdout
function executeCommand( opt ) {

  var sh = new ActiveXObject("WScript.Shell"); 

  if (opt.path) sh.CurrentDirectory = opt.path;

  var process = sh.exec( opt.cmd );

  if (opt.input) {
    process.StdIn.Write(opt.input);
    process.StdIn.Close();
    }

  do {
    WScript.sleep( 100 );
    } while (process.status === 0);

  var errors = process.StdErr.ReadAll();
  if (process.ExitCode != 0 || errors.match(/\S/)) {
    throw "Error\n" +
          opt.cmd + "\n" +
          "Command terminated with exit code " + process.ExitCode + "\n" +
          errors ;
    }

  return process.StdOut.ReadAll( ).replace(/\s*$/,"");;

  }

// Read a file completely into a string
function readFile( filename  ) {
  var fso = new ActiveXObject("Scripting.FileSystemObject");
  var fs = fso.OpenTextFile( filename, 1 );
  return fs.ReadAll();
  }
