/* Logic for astrotest.html */
var fso; 

function init() {  

  clearError();
  clearOut( );

  try {
    fso = new ActiveXObject("Scripting.FileSystemObject");
    } catch (e) {
    showError("This HTML page requires ActiveX and will only work correctly on Internet Explorer");
    }

  fill_select_options(houseSystems)(byId("hsys"));
  fill_select_options(dateFunctions)(byId("rand-date-fn"));

// Define the reactions on clicks and changes
  registerFor( "rand-date-fn", "change", rand_date_fn_changed );    
  registerFor( "compute", "click", compute );
  registerFor( "generate_seed", "click", generate_seed );
  
  var file_chooser = fileChooser( );
  registerFor( byClass("choose-file"), "click", file_chooser.open );
  registerFor( "file-dialog", "change", file_chooser.take );

  registerFor( byClass("juldate"), "change", input_juldate );
  registerFor( byClass("add_row"), "click", on_click_score_fn_add_row );

  registerFor( "save-plan", "click", function(event) {
    var plan = generatePlan();
    savePlan( getText("plan-file"), plan );      
    });    
  registerFor( "read-plan", "click", function(event) {
    var plan = readPlan( getText("plan-file") );
    fillSelectionsFromPlan( plan );
    });    
  registerFor( "switch-to-aaf", "click", function(event) {
    byId("row-csv").style.display="none";
    byId("row-aaf").style.display="";
    });  
  registerFor( "switch-to-csv", "click", function(event) {
    byId("row-csv").style.display="";
    byId("row-aaf").style.display="none";
    setText("source-file","");
    });  
  registerFor( "convert-from-aaf", "click", function(event) {
    try {
      clearError( );
      clearOut( );
      setText("source-file","");
      var file_csv = convert_aaf_to_csv( );
      out("File successfully converted to '"+file_csv+"'");
      setText("source-file",file_csv);
      byId("row-csv").style.display="";
      byId("row-aaf").style.display="none";
      } catch (e) {
           showError(e);
          }
    });  
  }

// Function for opening the file explorer value help, 
// and transporting the chosen file name to another input field
function fileChooser() {
  var id;
  return {
    open:function(event) {
      id = event.target.getAttribute("data-for");
      byId("file-dialog").click( );
      },
    take:function(event) {
      setText(id,event.target.value);
      }
    };
  }

function on_click_score_fn_add_row(event) {
  score_fn_add_row(event.target.id.replace(/^add-/,""))();
  }
  
// Different function to add score function terms  
function score_fn_add_row( id ) {
  var fn = {};
  if (!(id in fn)) {
    var cells = parseCellsFromTemplate( "row-template-"+id );
    fn[id] = function() {
      var row = byId("score-fn").insertRow(-1);
      setClass(row,id.replace(/^add\-/,""));    
      cells.each( function( cell ) {
        var td = row.insertCell();
        td.innerHTML = cell;
        });
      byClass("fn",row).each( function() {
        registerFor( this, "change", score_fn_changed);
        });  
      byClass("delete_row",row).each( function() { 
        registerFor( this, "click", delete_row );
        });  
      return row;
      };
  }    
  return fn[id];
  
  function parseCellsFromTemplate( template_id  ) {
    var tmpl = document.createElement("div");
    tmpl.innerHTML = "<table><tr>"+byId(template_id).innerHTML+"</tr></table>";
    fill_listboxes( tmpl ) ;
    return byTagName( "td", tmpl ).map( function(c) { return c.innerHTML });
    }    
  }

// Adapt the additional parameter input on change of random date function
function rand_date_fn_changed() {
  var fn = getText("rand-date-fn");  
  byId("R-dates").style.display = (fn == 'R' ? "" : "none" );
  byId("Q-quantiles").style.display = (fn == 'Q' ? "" : "none" );    
  }
  
// Adapt the input field visibility for a score function 
function score_fn_changed( ) {
  var row = getTagNamedParent(this,"TR");
  adaptScoreFunctionInputFields( row );
  }
  
// For a given score function: which fields are visible  
function adaptScoreFunctionInputFields( row ) {
  var fn = byClass("fn",row)[0].value;
  byClass( "pl2", row ).each( function() {
    this.style.display = takesTwoPlanets( fn ) ? "":"none";
    })      
  }  

// Describes which functions need two planets as arguments  
function takesTwoPlanets( fn ) {
  return (fn == "zll") || (fn == "mpp");
  }    

// On click: Delete a term of the score function
function delete_row( event ) {
  var row = getTagNamedParent( event.target, "tr" );
  row.parentElement.removeChild(row);
  }

// Fill all listboxes of a row
function fill_listboxes( element ) { 
  [ 
    { type:"planet",     options:planets },
    { type:"fn",         options:scoreFunctions },
    { type:"aspect",     options:aspects },
    { type:"house",      options:houses },
    { type:"sign",       options:signs },
    { type:"reference",  options:references },
    { type:"direction",  options:directions }
    ].each( function() {
      byClass( this.type, element).each( 
        fill_select_options( this.options )
        );
      });
}

// General function to fill a listbox with data
function fill_select_options( data ) {
  return function( selectElement ) {
    for (var key in data) {
      add_option( selectElement, key, data[key] );
      }
    }
  }

// General function to add a select option
function add_option( selectElement, key, value ) {
  var option = document.createElement("option");
  option.value = key;
  option.textContent = value;
  if ( selectElement.hasAttribute( "value" ) && 
       (key === selectElement.getAttribute("value")) 
     ) {
    option.setAttribute("selected", true);
    }
  selectElement.appendChild(option);
  }  
  
// Perform the computation, if everything is OK  
function compute() {
  try {
    if (checkInput()) {
      var plan = generatePlan();
      savePlan( getText("plan-file"), plan );
      clearOut( );
      executePlan();
      }
    } catch (e) {
      out (e);
      }
  }  

// Output text to output area (Log)
function out( text ) {
  byId("output").value += text + "\n";
  }

// Clear output area (Log)
function clearOut() {
  setText("output","");
  }

// All input rules to check before computation  
function checkInput( ) {
  clearError();
  try {
    
    if (is_initial("source-file")) throw {
      msg:"Please specify source file",
      field:byId("source-file")
      };
      
    if (!fso.FileExists(fullPath(getText("source-file")))) throw {
      msg:"File '" + fullPath(getText("source-file")) + "' not found",
      field:byId("source-file")      
      };
      
    byTagName("input")
      .subset( function() { 
        return (this.getAttribute("type") == "number") && 
               isVisible( this );
        })
      .each( check_numeric );     
      
    
    if (byTagName("tr",byId("score-fn")).length === 0) {
      throw {
        msg: "No score function defined!"        
        }
      } 
      
    if (getText("test-runs")<=0) {
      throw {
        msg:"Please specify a positive number of runs",
        field:byId("test-runs")
        }
      }
      
    if (getText("rand-date-fn")=="R") {
      var jd1 = check_numeric("jd1"),
          jd2 = check_numeric("jd2"),
          jd3 = check_numeric("jd3");
      
      if (jd1 >= jd2) throw {
        msg:"The julian date values have to increase",
        field: byId("jd2")
        };
      if (jd2 >= jd3) throw {
        msg:"The julian date values have to increase",
        field: byId("jd3")
        }
      
      }
      
      if (getText("rand-date-fn")=="Q") {
        var q = check_numeric("quantiles");
        if (q <=1) throw {
          msg:"Please choose at least two 2-quantile (=median)",
          field:"quantiles"
          };
        if (q != Math.floor(q)) throw {
          msg:"Please choose an integer value for quantile number",
          field:"quantiles"
          };          
        }    
    
    return true;
        
    } catch(e) {
      showError(e);
      return false;
      }          
  }

// Display an exception object in error message area,
// Mark a field as erroneous  
function showError( e ) {
  var msg = (typeof e == "string") ? e : e.msg,
      field;
  setText("msg",e.msg || e);
  if ((typeof e == "object") && 
       e.field && 
       (field = getElement( e.field ))) {
    field.select(); 
    field.focus();
    setClass(field,"error");
    }  
  }  
  
// Clear error message area, 
// clear all field error markers
function clearError() {
  setText("msg","");
  byTagName("input").concat(byTagName("select")).each( function() {
    resetClass(this,"error");
    });
  }  

// Checks whether element is currently visible
function isVisible( element ) {
// Quick tests  
  if (!element) element = this;
  if (element.offsetParent === null) return false;
  if (element.style.display == "none") return false;
  var row = getTagNamedParent(element,"TR");
  if (row && row.style.display == "none") return false;
// Slow, but definitive  
  var s = window.getComputedStyle( element );
  return (s.display != "none");
  }  

// Throws an exception if field content is not numeric
// If numeric, the field content is returned as a number  
function check_numeric( field ) {
  if (!field) field = this;
  if (typeof field == "string") field = byId(field);
  var v = field.value;
  if (!is_numeric(v)) {
    throw {
      msg:"Please enter a number",
      field:field
      };
    }  
  return 1*v;
  } 
 
// Checks whether "value" contains a number  
function is_numeric( value ) {
  return !isNaN(parseFloat((value))) && isFinite(value)
  }
  
  
// Returns true if field is initial
// i.e. contains only whitespace, or, 
// in the numeric case, is 0  
function is_initial( field ) {
 if (!field) field = this;
 if (typeof field == "string") field = byId(field);
 var v = field.value;
 if (is_numeric(v)) return ( v == 0);
 else return !v.match(/\S/); 
 }     

// For validation purposes: Throws an exception
// if field contains only whitespace (non-numeric case)
// or 0 (numeric case) 
function check_not_initial( field ) {
  if (!field) field = this;
  var v = field.value;
  if (is_numeric(v)) {
    if ( 1*v === 0) throw {
    msg:"Please enter non-zero value",
    field:field
    }
  }
  else {
    if (!v.match(/\S/)) throw {
    msg:"Please enter a value",
    field:field
    }
  }   
  
}
  
// Build the full execution plan from the HTML page input  
function generatePlan( ) {
  
// Build the plan - as an array of strings (to become the lines of the file)
  var plan = ["version:0.9"];
  
  plan.push( "source-file:"+getText("source-file") );
  plan.push( "test-runs:"+1*getText("test-runs") );
  plan.push( "hsys:"+byId("hsys").value );
  plan.push( "rand-date-fn:"+byId("rand-date-fn").value );
  switch (getText("rand-date-fn")) {
    case 'Q':
      plan.push( "quantiles:"+1*getText("quantiles") );
      break;
    case 'R':
      plan.push( "jd:"+1*getText("jd1")+","+1*getText("jd2")+","+1*getText("jd3") );
      break;
    }
  plan.push( "seed:"+1*getText("seed1")+","+1*getText("seed2") );
  plan.push( "dbg-level:"+getText("dbg-level") );
  plan.push( "cont-with-mosh:"+ (byId("cont-with-mosh").checked ? 1 : 0 ) );
  var ephepath = getText("ephepath");
  if (ephepath.match(/\S/)) {
    plan.push("ephepath:"+ephepath);
    }
  
  generateScoreFunction( plan );
  
  return plan;
  
  }
  
// Build the score function terms from the input in the HTML page  
function generateScoreFunction( plan ) {  
  
  var orb = getText("orb")*1;  // Needed for aspect resolution

  var translate = { 
    "aspect":function(row,plan) {
      var val = getInputValues(row);
      var fn = val[5] == 'z' ? raw_fn.zll : raw_fn.mpp;
      if (val[2]===0) {
        plan.push( fn( val[0], val[1], val[3], -orb, orb  ) );
        
        }
      else if (val[2] == 180) {
        plan.push( fn( val[0], val[1], val[3], 180-orb, 180  ) );
        plan.push( fn( val[0], val[3], val[1], -180, -180+orb ) );
        }
      else {  
        if (val[4]!='-') 
          plan.push( fn( val[0], val[1], val[3], val[2]-orb, val[2]+orb  ) );
        if (val[4]!='+') 
          plan.push( fn( val[0], val[3], val[1], val[2]-orb, val[2]+orb  ) );
      }
       
      }, 
    "house-position":function(row,plan) {
      var val = getInputValues(row);
      plan.push( raw_fn.mp( val[0],val[1],null,val[2],val[2]+30) );
      }, 
    "sign-position":function(row,plan) { 
      var val = getInputValues(row);
      plan.push( raw_fn.zl( val[0],val[1],null,val[2],val[2]+30) );
      }, 
    "raw-term":function(row,plan) {
      var values = getInputValues(row);
      var fn = values[2];
      values.splice(2,1);
      plan.push( raw_fn[fn].apply(this,values) );
      } 
   };
   
  var raw_fn = {
    "zl":function(coeff,pl1,pl2,arg1,arg2){ 
      return coeff+"*zl("+pl1+","+arg1+","+arg2+")";
      },
    "zll":function(coeff,pl1,pl2,arg1,arg2){
      return coeff+"*zll("+pl1+","+pl2+","+arg1+","+arg2+")";
      },
    "mp":function(coeff,pl1,pl2,arg1,arg2){
      return coeff+"*mp("+pl1+","+arg1+","+arg2+")";
      },
    "mpp":function(coeff,pl1,pl2,arg1,arg2){
      return coeff+"*mpp("+pl1+","+pl2+","+arg1+","+arg2+")";
      }
  }; 

  plan.push("score-source-begin");
  plan.push("orb:"+getText("orb"));
  byTagName("tr",byId("score-fn")).each( function(tr) {
    plan.push(tr.className+":"+getInputValues(tr).join(","))
    });
  plan.push("score-source-end");
  
  plan.push("score-begin");
  byTagName("tr",byId("score-fn")).each( function(tr) {
    translate[tr.className](tr,plan);
    });
  plan.push("score-end");
  
    
  }  
  
// Retrieves an array of all the inputs in a row  
function getInputValues(tr) {
  var values = [];
  getInputFields(tr).each( function() {
     var v = this.value;
     var value;
     if (v==="") value = 0;
     else if (is_numeric(v)) value = v * 1;
     else value = v;
     values.push( value );
     })
  return values;
  }

// Collect all the input fields and listboxes of a row in order
function getInputFields(tr) {
  return byCondition( function() {
    return this.tagName.match(/SELECT|INPUT/)
    }, tr )
  }

// Execute the C program "astrotest.exe" from the command line
function executePlan( ) {  
  out('cd "'+readPathFromURL()+'"');
  out('astrotest "'+getText("plan-file")+'" "'+getText("source-file")+'"');   
  var output = executeCommand({
    cmd:'astrotest "'+getText("plan-file")+'" "'+getText("source-file")+'"',
    path:readPathFromURL()
    });
  out(output);
  }        

// Generate two seed-values randomly
function generate_seed( ) {  
  setText("seed1", (Math.random( )*10000000).toFixed(0) );
  setText("seed2", (Math.random( )*10000000).toFixed(0) );  
  }
  
// Input fields which expect a julian date
// As a hint, the corresponding calendar date will be showed 
function input_juldate(event) {
  var inputField = event.target;
  
// If calendar date given: Convert to Julian date  
  var m = inputField.value.match(/(\d{1,2})\.(\d{1,2})\.(\-?\d{1,4})/);
  if (m) {
    var jd = swe_julday( m[3],m[2],m[1], 0, m[3] >= 1582 );
    inputField.value = jd;
    }    

// Make calendar date quickinfo
  inputField.setAttribute("title", 
    is_numeric(inputField.value) ? swe_revjul( 1*inputField.value ) : "" );

  }  
  

// Save plan to file
function savePlan( file, plan ) {
  var fs = fso.OpenTextFile( fullPath(file), 
                             2, // for Writing
                             1  // Create if not existing
                             );  
  plan.each( function(line) {
    fs.WriteLine( line );
    });
  fs.Close( );
  }
  
// Read plan from file  
function readPlan( file ) {
  var plan = [];
  var fs = fso.OpenTextFile( fullPath(file), 
                             1 // for Reading
                             );  
  while (!fs.AtEndOfStream) {
    plan.push( fs.ReadLine() );    
    }
  fs.Close(); 
  return plan; 
  }
  

// After having read the plan from file, the lines will
// be used to propagate the input fields of the HTML page
function fillSelectionsFromPlan( plan ) {
  deleteScoreFunction( );
  var readingScoreSource = false;
  var hasScoreSource = false;
  var readingScore = false; 
  var score = [];
  plan.each( function(line) {
    if (/^#/.test(line)) return;
    var m = line.match( /^\s*([\w-]+):\s*(.*)/ );
    if (m && !readingScoreSource) {
      switch(m[1]) {
        case "test-runs":
        case "hsys":
        case "rand-date-fn":
        case "quantiles":
        case "dbg-level":
        case "source-file":
        case "ephepath":
          setText(m[1],m[2]);
          return;
        case "cont-with-mosh":
          byId("cont-with-mosh").checked = (m[2] != "0");
          return;
        case "seed":
          var seeds = m[2].split(",");
          setText("seed1",seeds[0]);
          setText("seed2",seeds[1]);
          return;
        case "jd":
          var dates = m[2].split(",");
          setText("jd1",dates[0]);  
          setText("jd2",dates[1]);  
          setText("jd3",dates[2]);
          return;  
        }
      }
    else if (/score\-source\-begin/.test(line)) {
      readingScoreSource = true;
      }    
    else if (/score\-source\-end/.test(line)) {
      readingScoreSource = false;
      }
    else if (/score\-begin/.test(line)) {
      readingScore = true;
      }    
    else if (/score\-end/.test(line)) {
      readingScore = false;
      }
    else if (readingScoreSource) {
      if (/^\s*orb\s*:\s*(.*)/.test(line)) {
        setText("orb",RegExp.$1);
        }
      else {
        hasScoreSource = true;
        addScoreSelectionFromPlan(line);  
        }
      }
    else if (readingScore) {
      score.push( line );      
      }
    });
    
// Adapt visibility of rand-date-fn parameters    
    rand_date_fn_changed();
    
// No score-source found? Use score
    if (!hasScoreSource && (score.length > 0)) {
      score.each( function(line) {
        var args = line.
// Replace trailing ")"        
                    replace(/\s*\)\s*$/,"").
// Split into value array, separated by comma, ), or *  
                    split(/\s*[*,()]\s*/);
// Add "raw-term" type row to score function
        var row = score_fn_add_row( "raw-term" )();
// Build an array for providing with values        
//                     coeff    pl1      fn
        var values = [ args[0], args[2], args[1] ];        
// Second planet (pl2), if function requires two         
        values.push( takesTwoPlanets(args[1]) ? args[3] : "");
// Append two numeric arguments          
        values.push.apply( values, args.slice(-2) );
// Fill values into row's input/select fields        
        getInputFields(row).each( function(field,i) {
          field.value = values[i];
          });  
        adaptScoreFunctionInputFields( row );                  
        });
      }
  }  
  
// Deletes all terms of the score function  
function deleteScoreFunction( ) {
  var score_fn = byId("score-fn");
  byTagName("tr",score_fn).each( function() {
    score_fn.deleteRow(-1);
    });
  }  

// Use score-source to propagate a term of the score function in UI
function addScoreSelectionFromPlan(line) {
  var m = line.match(/([\w-]+)\s*:\s*(.*)/);
  if (m) {
    var row = score_fn_add_row( m[1] )();
    var value = m[2].split(",");    
    getInputFields(row).each( function(field,i) {
      field.value = value[i];
      });  
    }    
  }    

// Convert .aaf file to .csv file, extracting the three relevant data
// (julian date, geographical longitude, geographical latitude)  
function convert_aaf_to_csv() {
  var file_aaf = fullPath(getText("source-file-aaf"));
  if (!file_aaf.match(/\S/)) {
    throw { 
      msg:"Please specify AAF source file",
      field:"aaf-source-file"
      };
    }
  if (file_aaf.match(/\.csv/)) {
    throw { 
      msg:"AAF source file should not end with .csv",
      field:"aaf-source-file"
      };
    }
  if (!fso.FileExists(file_aaf)) {
    throw {
      msg: "File '" + file_aaf + "' could not be found",
      field:"aaf-source-file" 
      };
    }
    
  var aaf = fso.OpenTextFile( fullPath(file_aaf), 
                             1 // for Reading
                             );
                                
  var file_csv = (file_aaf.indexOf(".")>=0) ? 
    file_aaf.replace(/\.([^.]*)$/,".csv")
    : file_aaf + ".csv";
    
  var csv = fso.OpenTextFile( fullPath(file_csv), 
                             2, // for Writing
                             1  // Create if not existing
                             );  
  var rc = aaf2csv(aaf,csv,stderr());
  csv.Close( );
  aaf.Close( );
  if (rc != 0) {
    throw {
      msg:"Errors while trying to convert AAF file (see log)",
      field:"aaf-source-file"
      }
    }
  else return file_csv;  
  
  function stderr() {
    return {
      WriteLine:function(line) {
        out(line);
        }
    }
  }  
}  

function fullPath( file ) {
  return (file.indexOf('\\')>=0) ? file : readPathFromURL()+"\\"+file;
}
  
function readPathFromURL() {
  return decodeURIComponent( 
    document.location.pathname.
      replace(/^\/(.*)\/.*\.html/,"$1").
      replace(/\//g,"\\")
      );
  }  


// General function to execute a command from within an HTML page  
function executeCommand( opt ) {

  var sh = new ActiveXObject("Wscript.Shell");

  if (opt.path) sh.CurrentDirectory = opt.path;

  var process = sh.exec( opt.cmd );

  if (opt.input) {
    process.StdIn.Write(opt.input);
    process.StdIn.Close();
    }

  waitUntilFinished( );

  var errors = process.StdErr.ReadAll();
  if (process.ExitCode != 0 || errors.match(/\S/)) {
    throw "Error\n" +
          opt.cmd + "\n" +
          "Command terminated with exit code " + process.ExitCode + "\n" +
          errors ;
    }

  return process.StdOut.ReadAll( ).replace(/\s*$/,"");;

  function waitUntilFinished( ) {
    if (process.status === 0) window.setTimeout( waitUntilFinished, 200);
    }

  }
  
// Julian Date <--> Calendar Date conversions, 
// Copied from Swiss Ephemeris (swedate.c) and adapted to JavaScript
function swe_julday(year, month, day, ut, useJulianCalendar) {
  var jd;
  var u,u0,u1,u2;
  u = year*1;
  if (1*month < 3) u -=1;
  u0 = u + 4712.0;
  u1 = month*1 + 1.0;
  if (u1 < 4) u1 += 12.0;
  jd = Math.floor(u0*365.25)
     + Math.floor(30.6*u1+0.000001)
     + 1*day + ut/24.0 - 63.5;
  if (!useJulianCalendar) {
    u2 = Math.floor(Math.abs(u) / 100) - Math.floor(Math.abs(u) / 400);
    if (u < 0.0) u2 = -u2;
    jd = jd - u2 + 2;            
    if ((u < 0.0) && (u/100 == floor(u/100)) && (u/400 != floor(u/400)))
      jd -=1;
  }
  return jd;
}  

function swe_revjul(jd, julianCalendar)
{
  var u0,u1,u2,u3,u4;  
  u0 = 1*jd + 32082.5;
  if (!julianCalendar) {
    u1 = u0 + Math.floor (u0/36525.0) - Math.floor (u0/146100.0) - 38.0;
    if (jd >= 1830691.5) u1 +=1;
    u0 = u0 + Math.floor (u1/36525.0) - Math.floor (u1/146100.0) - 38.0;
  }
  u2 = Math.floor (u0 + 123.0);
  u3 = Math.floor ( (u2 - 122.2) / 365.25);
  u4 = Math.floor ( (u2 - Math.floor (365.25 * u3) ) / 30.6001);
  var month = Math.floor(u4 - 1.0);
  if (month > 12) month -= 12;
  var day = Math.floor((u2 - Math.floor (365.25 * u3) - Math.floor (30.6001 * u4)));
  var year = Math.floor((u3 + Math.floor ( (u4 - 2.0) / 12.0) - 4800));
  return day + "." + month + "." + year;
}

  
  
registerFor(window,"DOMContentLoaded",init);