// --- Code unter BSD-Lizenz, Copyright Ruediger Plantiko (2011)

// ----------------------------------------------------------------------
// Die essentiellen browseruebergreifenden JS-Funktionen
// ----------------------------------------------------------------------

// --- each() Array-Iteration mit each()
// Die Callbackfunktion erhaelt den Wert des Array-Elements als "this"
// Als Argument wird der aktuelle Array-Index uebergeben
// Kompatibel zu "Prototype"
if (!this.$break) { $break = {};  } // Spezielle Exception, um aus each() auszubrechen
Array.prototype.each = function(f,context) {
  var i,n=this.length;
  try {
    for (i=0;i<n;++i) {
      f.call(context||this[i],this[i],i);
      }
    }
    catch(e) {
      if (e!=$break) {throw e; }
      }
  };

// --- subset(filter)
// Die Teilmenge aller Arrayelemente bilden, fuer die die filter-Funktion true ergibt
Array.prototype.subset = function(filter) {
  var result = [];
  this.each( function() {
    if (filter.call(this)) { result.push(this); }
    });
  return result;
  };

// --- Browseruebergreifender HTTP-Request
// callback,data,action,headerFields sind optional
function doRequest(url,callback,data,action,headerFields) {
  var field,value,
      theData   = data || null,
      requestor =  getRequestor();
  requestor.open(action||"GET",url,!!callback);  // Synchron <=> kein Callback
  if (callback) { requestor.onreadystatechange = function() {
     if (requestor.readyState ==4) {
       callback.call(requestor);  // Mit requestor = this aufrufen
       }
     };
    }
  if (headerFields) {
    for (field in headerFields) {
       requestor.setRequestHeader(field,headerFields[field]);
       }
    }
  requestor.send(theData);
  return callback ? requestor : requestor.responseText;
}

// --- Ein  XMLHTTPRequest-Objekt beschaffen
function getRequestor() {
  try {
// Mozilla, Opera, Safari, MSIE >= 7
    return new XMLHttpRequest();
    } catch(e) {
    try {
// MSIE >= 6
        return new ActiveXObject("Microsoft.XMLHTTP");
    } catch(e) {
        try {
// MSIE >= 5
          return new ActiveXObject("Msxml2.XMLHTTP");
          } catch(e) {
         alert("HTTP-Requestobjekt kann nicht erzeugt werden");
        }
      }
    }
  }

// --- Shortcut fuer ElementById
function byId(id) {
  return document.getElementById(id);
  }

// --- Shortcut fuer ElementsByName (auch auf Element-Ebene)
function byName(name,theParent) {
  return byCondition( function() { return (this.name == name); }, theParent);
  }

// --- Das erste Element des angegebenen Namens
function firstByName(name,theParent) {
  var elems = byName(name,theParent);
  return elems.length > 0 && elems[0] || null;
  }

// --- Elemente einer CSS-Klasse
if (document.getElementsByClassName) {
  byClass = function(className,theParent) {
    var nodeList,i,n,result=[];
    nodeList = (theParent||document).getElementsByClassName(className);
    for (i=0,n=nodeList.length;i<n;++i) result.push(nodeList[i]);
    return result;
    };
  }
else {
  byClass = function(className,theParent) {
  var classPattern = new RegExp( "\\b" + className + "\\b");
  return byCondition( function() { return this.className.match(classPattern); }, theParent);
  };
}

// --- Shortcut fuer getElementsByTagName
function byTagName(tagname,theParent) {
  var nodeList,i,n,result=[];
// Wir verwenden die native Funktion, muessen dafuer aber NodeList -> Array mappen
  nodeList = (theParent||document).getElementsByTagName(tagname);
  for (i=0,n=nodeList.length;i<n;++i) result.push(nodeList[i]);
  return result;
  }


// --- Uebergeordnetes Element mit einem bestimmten Elementnamen finden
function getTagNamedParent(elem,parentTagName) {
  var parentTagNamePattern = new RegExp( parentTagName, "i" );
  return getParentByCondition( elem, function() {
    return this.nodeName && this.nodeName.match(parentTagNamePattern);
    } );
  }

// --- Uebergeordnetes Element mit einer Bedingung finden
function getParentByCondition(elem,condition) {
  var node = elem;
  while (node) {
    node = node.parentNode;
    if (condition.call(node)) {
      return node;
      }
    }
  return null;
  }

// --- Alle Elemente, die eine Bedingung erfuellen
function byCondition( condition, theParent) {
  var children, n, i, result = [];
  children = (theParent || document).childNodes;
  n = children.length;
  for (i=0;i<n;++i) {
    if (children[i].nodeType==1) {
      if (condition.call(children[i])) { result.push( children[i] ); }
      Array.prototype.push.apply( result,
        byCondition( condition, children[i] ) );
      }
    }
  return result;
  }

// --- Browseruebergreifende Registrierung einer Funktion
function registerFor( theElement, theEvent, theHandler ) {
  
  if (theElement instanceof Array) {
    return theElement.each( function(el) {
      registerFor( el, theEvent, theHandler );
      });
    }
    
  if (typeof theElement == "string") {
    return registerFor( byId(theElement), theEvent, theHandler );
    }  
  
  if (typeof theHandler != "function") {
    alert( "Programmfehler: Nur Funktionen koennen registriert werden");
    return;
    }

  var standard = !! window.addEventListener;
  var eventNormalized = normalizeEventName(theEvent,standard);

  var f = function(e) {
    theHandler.call(getSource(e),e);
    };

  if (standard) {
    theElement.addEventListener(eventNormalized, f, false);
    }
  else {
    theElement.attachEvent(eventNormalized, f );
    }
  return f; // Fuer allfaellige Deregistrierung
  }

// --- Browseruebergreifende Deregistrierung
function unregister( theElement, theEvent, f ) {
  var standard = !! window.removeEventListener;
  var eventNormalized = normalizeEventName(theEvent,standard);
  if (standard) {
    theElement.removeEventListener(eventNormalized,f,false);
    }
  else {
    theElement.detachEvent(eventNormalized,f,false);
    }
  }

// --- Browseruebergreifendes Event-Stop
function stopEventPropagation( theEvent ) {
  var e = theEvent || window.event;
  if (e.stopPropagation) e.stopPropagation();
  else e.cancelBubble = true;
  }

function preventDefaultHandler( theEvent ) {
  var e = theEvent || window.event;
  if (e.preventDefault) e.preventDefault();
  else e.returnValue = false;
  }

function normalizeEventName( theEvent, standard ) {
  return standard ?
    theEvent.replace(/^on/,"") :
    theEvent.replace(/^(?!on)/,"on") ;
  }

// --- Browseruebergreifende Ermittlung einer Event-Source
function getSource( theEvent ) {
  return theEvent && theEvent.target || window.event && window.event.srcElement;
  }

// --- Browseruebergreifende Ermittlung eines key-Codes
function getKeyCode(theEvent) {
  var e = theEvent || window.event;
  return e.keyCode || e.which;
  }

// --- Element mittels seiner ID ODER seines Namens beschaffen
// Die ID hat dabei Vorrang
function getNodeFromIdOrName(idOrName) {
  return byId(idOrName) || firstByName(idOrName);
  }

function getElement( idOrNode ) {
  return ( typeof idOrNode == "string" || 
           idOrNode instanceof String ) ?
    getNodeFromIdOrName(idOrNode) : idOrNode;
  }

// --- Einen Text ins DOM setzen
// idOrNode ist entweder eine id oder ein Elementknoten
// Der Text wird im textfoermigen Kindknoten gepflegt
// Wenn ein solcher nicht existiert, wird er zuerst
// noch angelegt
// Ist das Element ein Inputfeld, so wird der Text in
// dessen Value-Attribut gestellt
function setText(idOrNode,text) {
  var textNode;
  var elem = getElement( idOrNode );
  if (elem) {
    if (elem.nodeName == "INPUT" || 
        elem.nodeName == "SELECT" ||
        elem.nodeName == "TEXTAREA" ) {
      elem.value = text;
      }
    else {
      textNode = elem.firstChild;
      if (textNode && (textNode.nodeType==3)) {
        textNode.data = text;
        }
      else {
        textNode = document.createTextNode(text);
        elem.appendChild(textNode);
        }
      }
    }
  }

// --- Einen Text aus dem DOM abholen
// Dies ist die zu setText() reziproke Methode, um Texte aus dem
// aktuellen Dokument zu lesen. Liefert fuer input-Felder den
// Wert des value-Attributs. Fuer alle anderen den Wert des
// ersten, als textfoermig angenommenen Kindknotens (oder "", wenn
// kein solcher existiert).
function getText(idOrNode) {
  var textNode;
  var elem = getElement( idOrNode );

  if (elem) {
    if (elem.nodeName == "INPUT" ||
        elem.nodeName == "TEXTAREA" ||
        elem.nodeName == "SELECT" ) { 
      return elem.value; 
      }
    textNode = elem.firstChild;
// Wenn das erste Kind ein Textknoten ist
    if (textNode && (textNode.nodeType==3)) { return textNode.data; }
    }

  return "";
  }

// --- Navigation zu einer URL mit Uebergabe von Formularfeldern
// Diese Felder verschmutzen nicht als Get-Parameter die URL, sondern
// werden im Request mittels adhoc-Formular als POST-Felder gesendet.
function gotoURL(url, fields) {

  var name, field, domPort,domPortOwn;
// Ggf. pruefen, ob Navigation erlaubt
  if (self.gNoRedirect) {
// Navigation auf andere Domaenen/Ports nicht erlaubt
    if (url.match(/^https?:\/\/([\w.:]+)/)) {
      domPort    = RegExp.$1;
      domPortOwn = document.location.href.match(/^https?:\/\/([\w.:]+)/)[1];
      if (domPort != domPortOwn) {
        alert( msgText.noRedirect ||
               "Umleitung auf ein anderes System ist nicht zugelassen");
        isBusy = false;
        return;
        }
      }
    }

// adhoc-Formular erzeugen, befuellen und abschicken
  var submitter = document.createElement("form");
  setAttributes(submitter,{action:url,method:"post"});

// Formular mit versteckten Formularfeldern abfuellen
  for (name in fields) {
    field = document.createElement("input");
    setAttributes(field,{type:"hidden",name:name,value:fields[name]});
    submitter.appendChild(field);
    }
  document.body.appendChild(submitter);
  submitter.submit();
}

// --- CSS-Klasse eines Elements setzen oder entfernen
function setClass(elem,theClass) {
   var cl = elem.className;
   if (!cl) {
      elem.className = theClass;
      return;
      }
   else if (!hasClass(elem,theClass)) {
      elem.className = cl + " " + theClass;
      }
   }
function resetClass(elem,theClass) {
   var cl = elem.className;
   if (cl) {
     elem.className = cl.replace( new RegExp("\\b"+theClass+"\\b","g"), "" ).
                         replace(/^\s+/,"").
                         replace(/\s+/g," ").
                         replace(/ $/,"");
     }                         
   }
function hasClass(elem,theClass) {
  return !! elem.className.match(  new RegExp("\\b"+theClass+"\\b") );
  }

// --- Attribute in einem Element setzen
function setAttributes(elem,atts) {
  var attName;
  for (attName in atts) {
    elem[attName] = atts[attName];
    }
  }

// Empfohlener Vererbungsmechanismus der MDC
function extend(childType, parentType) {
  var p,proto;
  if (typeof childType.prototype.__proto__ != "undefined") {
    childType.prototype.__proto__ = parentType.prototype;
    }
  else {
    for (p in parentType.prototype) {
      if (typeof childType.prototype[p] == "undefined")
        childType.prototype[p] = parentType.prototype[p];
      }
    }
  return childType;
}