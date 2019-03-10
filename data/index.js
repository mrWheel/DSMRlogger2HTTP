
//*************************************************************************  
//  Program  : index.js, part of DSMRlogger2HTTP
//  Version  : v0.7.6
//
//  Copyright (c) 2018 Willem Aandewiel
//
//  TERMS OF USE: MIT License. See bottom of file.                                                            
//*************************************************************************      

    //<script> 		
      $(document).ready(function(){
        console.log("document.ready() and loaded..");
        // hide the button
        $('#hiddenButton').hide();
        // set the pane to load if button is clicked
      	$('.next').click(function(){
      	      $('.nav-tabs a[href="#tab_Actual"]').tab('show');
        });
        // now trigger a click on the buttun
        $('.next').trigger('click');
      });

			var tab_pane;
      // Force DSMRState first launch
      var Timer_updateDSMR;
      var firstDSMR = true;
      var Timer_updateWeek;
      var firstWeek  = true;
      var Timer_updateHours;
      var firstHour  = true;
      var Timer_updateMonths;
      var firstMonth = true;
      var Timer_updateDeviceInfo;
      var firstDeviceInfo  = true;
   

	  $('a[data-toggle=\"tab\"]').on('shown.bs.tab', function (e) {   
	  	console.log("data-toggle.on('shown.bs.tab' ....");
        // remove all the timers when we change tabs
        clearInterval(Timer_updateDSMR);  
        clearTimeout(Timer_updateWeek);  
        clearTimeout(Timer_updateHours);  
        clearTimeout(Timer_updateMonths);  
        clearTimeout(Timer_updateDeviceInfo);  
        tab_pane = $(e.target).attr("href")  
        console.log('activated ' + tab_pane );  
      
        // IE10, Firefox, Chrome, etc.
        if (history.pushState) 
        	window.history.pushState(null, null, tab_pane);
        else 
           window.location.hash = tab_pane;
           
        if (tab_pane=='#tab_Actual') {	//-- || tab_pane=='') {
          console.log('new pane is #tab_Actual');
          firstDSMR = true;
          updateDSMRactual();
          console.log("setInterval(updateDSMRactual, 10000)");
          Timer_updateDSMR = setInterval(updateDSMRactual, 10000); 	// 10000 MS == 10 seconden
        }
	    
        if (tab_pane=='#tab_Week')  {
          console.log('new pane is #tab_Week');
          $('#table_Week').bootstrapTable('refresh',{silent:true, url:'/getTableWeek.json'}); 
          firstWeek = true;
          updateWeek();
          Timer_updateWeek = setInterval(updateWeek, 60000); 		// 60000 MS == 1 minuut
        }  
	    
        if (tab_pane=='#tab_Hour')  {
          console.log('new pane is #tab_Hour');
          $('#table_Hours').bootstrapTable('refresh',{silent:true, url:'/getTableHours.json'}); 
          firstHour = true;
          Timer_updateHours = setInterval(updateHours, 20000); 		// 20000 MS == 20 seconden
        }  
	    
        if (tab_pane=='#tab_Month')  {
          console.log('new pane is #tab_Month');
          $('#table_Months').bootstrapTable('refresh',{silent:true, url:'/getTableMonths.json'}); 
          firstMonth = true;
          Timer_updateMonths = setInterval(updateMonths, 30000); 		// 30000 MS == 30 seconden
        }  
	    
        if (tab_pane=='#tab_DeviceInfo')  {
          console.log('new pane is #tab_DeviceInfo');
          firstDeviceInfo = true;
          updateDeviceInfo();
          console.log("setInterval(updateDeviceInfo, 60000)");
          Timer_updateDeviceInfo = setInterval(updateDeviceInfo, 60000); 		// 60000 MS == 1 minuut
        }  
        
      });	// $('a[data-toggle=\"tab\"]') ..

      
      function updateDSMRactual(){
        // only if chart panel is active
        if (tab_pane=='#tab_Actual' | firstDSMR ){
          if (firstDSMR) {
          	console.log("updateDSMRactual() for the first time");
          }
          firstDSMR = false;
          $.getJSON('/getActual.json', function(data){
          	console.log("updateDSMRactual : " + JSON.stringify(data) );
            $('#DSMRtimestamp').html(data.Timestamp);
            $('#DSMRenergyDelivered').html(data.Energy_Delivered);
            $('#DSMRenergyReturned').html(data.Energy_Returned);
            $('#DSMRgasDelivered').html(data.Gas_Delivered);
            $('#DSMRenergyDeliveredTariff1').html(data.Energy_Delivered_Tariff1);
            $('#DSMRenergyDeliveredTariff2').html(data.Energy_Delivered_Tariff2);
            $('#DSMRenergyReturnedTariff1').html(data.Energy_Returned_Tariff1);
            $('#DSMRenergyReturnedTariff2').html(data.Energy_Returned_Tariff2);
            $('#DSMRpowerDelivered').html(data.Power_Delivered);
            $('#DSMRpowerReturned').html(data.Power_Returned);
            $('#DSMRvoltage_l1').html(data.Voltage_l1);
            $('#DSMRcurrent_l1').html(data.Current_l1);
            $('#DSMRvoltage_l2').html(data.Voltage_l2);
            $('#DSMRcurrent_l2').html(data.Current_l2);
            $('#DSMRvoltage_l3').html(data.Voltage_l3);
            $('#DSMRcurrent_l3').html(data.Current_l3);
            $('#DSMRpowerDelivered_l1').html(data.Power_Delivered_l1);
            $('#DSMRpowerReturned_l1').html(data.Power_Returned_l1);
            $('#DSMRpowerDelivered_l2').html(data.Power_Delivered_l2);
            $('#DSMRpowerReturned_l2').html(data.Power_Returned_l2);
            $('#DSMRpowerDelivered_l3').html(data.Power_Delivered_l3);
            $('#DSMRpowerReturned_l3').html(data.Power_Returned_l3);
          }).fail(function(err){
            console.log("err getJSON getActual.json "+JSON.stringify(err));
          });
        }
      };
      
      function updateWeek(){
        // only if chart panel is active
        if (tab_pane=='#tab_Week' | firstWeek ){
          if (firstWeek) {
          	console.log("updateWeek() for the first time");
          }
          updateTableWeek();
          firstWeek = false;
        }
      };

      function updateTableWeek(){
        // only if chart panel is active
        $('#table_Week').bootstrapTable('refresh',{silent: true, showLoading: false, url: '/getTableWeek.json'});
      };

      function updateHours(){
        // only if chart panel is active
        if (tab_pane=='#tab_Hour' | firstHour ){
          if (firstHour) {
          	console.log("updateHours() for the first time");
          }
          updateTableHours();
          firstHour = false;
        }
      };

      function updateTableHours(){
        // only if chart panel is active
        $('#table_Hours').bootstrapTable('refresh',{silent: true, showLoading: false, url: '/getTableHours.json'});
      };

      function updateMonths(){
        // only if chart panel is active
        if (tab_pane=='#tab_Month' | firstMonth ){
          if (firstMonth) {
          	console.log("updateMonths() for the first time");
          }
          updateTableMonths();
          firstMonth = false;
        }
      };

      function updateTableMonths(){
        // only if chart panel is active
        $('#table_Months').bootstrapTable('refresh',{silent: true, showLoading: false, url: '/getTableMonths.json'});
      };

      
      function updateDeviceInfo(){
        // only if chart panel is active
        if (tab_pane=='#tab_DeviceInfo' | firstDeviceInfo ){
          if (firstDeviceInfo) {
          	console.log("updateDeviceInfo() for the first time");
          }
          firstDeviceInfo = false;
          $.getJSON('/getDeviceInfo.json', function(data){
          	console.log("updateDeviceInfo : " + JSON.stringify(data) );
            $('#D_identification').html(data.Identification);
            $('#D_p1_Version').html(data.P1_Version);
            $('#D_electricityTariff').html(data.Electricity_Tariff);
            $('#D_equipmentId').html(data.Equipment_Id);
            $('#D_gasDeviceType').html(data.Gas_Device_Type);
            $('#D_gasEquipmentId').html(data.Gas_Equipment_Id);
            $('#D_Author').html(data.Author);
            $('#D_FwVersion').html(data.FwVersion);
            $('#D_Compiled').html(data.Compiled);
            $('#D_FreeHeap').html(data.FreeHeap);
            $('#D_ChipID').html(data.ChipID);
            $('#D_CoreVersion').html(data.CoreVersion);
            $('#D_SdkVersion').html(data.SdkVersion);
            $('#D_CpuFreqMHz').html(data.CpuFreqMHz);
            $('#D_SketchSize').html(data.SketchSize);
            $('#D_FreeSketchSpace').html(data.FreeSketchSpace);
            $('#D_FlashChipID').html(data.FlashChipID);
            $('#D_FlashChipSize').html(data.FlashChipSize);
            $('#D_FlashChipRealSize').html(data.FlashChipRealSize);
            $('#D_FlashChipSpeed').html(data.FlashChipSpeed);
            $('#D_FlashChipMode').html(data.FlashChipMode);
            $('#D_BoardType').html(data.BoardType);
            $('#D_SSID').html(data.SSID);
            $('#D_PskKey').html(data.PskKey);
            $('#D_IpAddress').html(data.IpAddress);
            $('#D_Hostname').html(data.Hostname);
            $('#D_upTime').html(data.upTime);
            $('#D_TelegramCount').html(data.TelegramCount);
            $('#D_TelegramErrors').html(data.TelegramErrors);
            $('#D_lastReset').html(data.lastReset);
          }).fail(function(err){
            console.log("err getJSON getDeviceInfo.json "+JSON.stringify(err));
          });
        }
      };

    //</script>

//***************************************************************************
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
// OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
// THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// 
//***************************************************************************

