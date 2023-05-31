function doGet(e) { 
  if(e!=undefined){
  var result = 'Ok'; // assume success
  if (e.parameter == undefined) {
    result = 'No Parameters';
  }
  else {

    var sheet_id = '1feByiI4lip0q4ODRvIWoWGAScQg90u9Uqrvls-M8uos';		// Spreadsheet ID
    var sheet = SpreadsheetApp.openById(sheet_id).getActiveSheet();		// get Active sheet
    var newRow = sheet.getLastRow() + 1;		
    var rowData = new Array(11);
    rowData[0] = new Date(); // Timestamp in column A
    try{  
    for (var param in e.parameter) {
      Logger.log('In for loop, param=' + param);
      var values = stripQuotes(e.parameter[param]);
      Logger.log(param + ':' + e.parameter[param]);
      // var valueArr = values.split("+");             								 
      switch (param) {
        case 'A': //Parameter
        
          rowData[1] = values; //Value in column B
          result = 'Written on column B';
          break;
        case 'B': //Parameter
        
          rowData[2] = values; //Value in column C
          
          result += ' ,Written on column C';
          break;
        case 'C': //Parameter
          rowData[3] = values; //Value in column C
      
          result += ' ,Written on column D';
          break;
        case 'D': //Parameter
        
          rowData[4] = values; //Value in column C
          result += ' ,Written on column E';
         
          break;
        
        case 'issue': //Parameter
        

          rowData[9] = values; //Value in column C
          
          result += ' ,Written on column J';
          break;
        case 'device': //Parameter
        

          rowData[10] = values; //Value in column C
          result += ' ,Written on column K';
          
          break;    
        default:
          result = "unsupported parameter";
          break;
      }
    }
    }catch( ex)
    {
      newRow=newRow+1;
     var range = sheet.getRange(newRow, 1,1,1);
    range.setValue(ex.message);
    }
    
    
    //Logger.log("Row data entered ===> " + JSON.stringify(rowData));
     var newRange = sheet.getRange(newRow, 1,1, rowData.length);
    //var newRange = sheet.getRange(newRow, 1,1,11);
    var rowData2D = new Array(1);
    rowData2D[0] = rowData;
    newRange.setValues(rowData2D);
  }

  // Return result of operation
  return ContentService.createTextOutput(result);
}
}
/**
* Remove leading and trailing single or double quotes
*/
function stripQuotes( value ) {
  return value.replace(/^["']|['"]$/g, "");
}
