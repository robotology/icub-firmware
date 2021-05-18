function CodeDefine() { 
this.def = new Array();
this.def["ert_main.cpp:Current_FOC_Obj"] = {file: "ert_main_cpp.html",line:24,type:"var"};
this.def["rt_OneStep"] = {file: "ert_main_cpp.html",line:38,type:"fcn"};
this.def["main"] = {file: "ert_main_cpp.html",line:75,type:"fcn"};
this.def["step"] = {file: "Current_FOC_cpp.html",line:23,type:"fcn"};
this.def["initialize"] = {file: "Current_FOC_cpp.html",line:159,type:"fcn"};
this.def["terminate"] = {file: "Current_FOC_cpp.html",line:165,type:"fcn"};
this.def["getRTM"] = {file: "Current_FOC_cpp.html",line:187,type:"fcn"};
this.def["Integrator_DSTATE"] = {file: "Current_FOC_h.html",line:41,type:"var"};
this.def["int8_T"] = {file: "rtwtypes_h.html",line:51,type:"type"};
this.def["uint8_T"] = {file: "rtwtypes_h.html",line:52,type:"type"};
this.def["int16_T"] = {file: "rtwtypes_h.html",line:53,type:"type"};
this.def["uint16_T"] = {file: "rtwtypes_h.html",line:54,type:"type"};
this.def["int32_T"] = {file: "rtwtypes_h.html",line:55,type:"type"};
this.def["uint32_T"] = {file: "rtwtypes_h.html",line:56,type:"type"};
this.def["real32_T"] = {file: "rtwtypes_h.html",line:57,type:"type"};
this.def["real64_T"] = {file: "rtwtypes_h.html",line:58,type:"type"};
this.def["real_T"] = {file: "rtwtypes_h.html",line:64,type:"type"};
this.def["time_T"] = {file: "rtwtypes_h.html",line:65,type:"type"};
this.def["boolean_T"] = {file: "rtwtypes_h.html",line:66,type:"type"};
this.def["int_T"] = {file: "rtwtypes_h.html",line:67,type:"type"};
this.def["uint_T"] = {file: "rtwtypes_h.html",line:68,type:"type"};
this.def["ulong_T"] = {file: "rtwtypes_h.html",line:69,type:"type"};
this.def["char_T"] = {file: "rtwtypes_h.html",line:70,type:"type"};
this.def["uchar_T"] = {file: "rtwtypes_h.html",line:71,type:"type"};
this.def["byte_T"] = {file: "rtwtypes_h.html",line:72,type:"type"};
this.def["pointer_T"] = {file: "rtwtypes_h.html",line:90,type:"type"};
}
CodeDefine.instance = new CodeDefine();
var testHarnessInfo = {OwnerFileName: "", HarnessOwner: "", HarnessName: "", IsTestHarness: "0"};
var relPathToBuildDir = "../ert_main.c";
var fileSep = "\\";
var isPC = true;
function Html2SrcLink() {
	this.html2SrcPath = new Array;
	this.html2Root = new Array;
	this.html2SrcPath["ert_main_cpp.html"] = "../ert_main.cpp";
	this.html2Root["ert_main_cpp.html"] = "ert_main_cpp.html";
	this.html2SrcPath["Current_FOC_cpp.html"] = "../Current_FOC.cpp";
	this.html2Root["Current_FOC_cpp.html"] = "Current_FOC_cpp.html";
	this.html2SrcPath["Current_FOC_h.html"] = "../Current_FOC.h";
	this.html2Root["Current_FOC_h.html"] = "Current_FOC_h.html";
	this.html2SrcPath["Current_FOC_private_h.html"] = "../Current_FOC_private.h";
	this.html2Root["Current_FOC_private_h.html"] = "Current_FOC_private_h.html";
	this.html2SrcPath["Current_FOC_types_h.html"] = "../Current_FOC_types.h";
	this.html2Root["Current_FOC_types_h.html"] = "Current_FOC_types_h.html";
	this.html2SrcPath["rtwtypes_h.html"] = "../rtwtypes.h";
	this.html2Root["rtwtypes_h.html"] = "rtwtypes_h.html";
	this.html2SrcPath["mw_cmsis_h.html"] = "../mw_cmsis.h";
	this.html2Root["mw_cmsis_h.html"] = "mw_cmsis_h.html";
	this.getLink2Src = function (htmlFileName) {
		 if (this.html2SrcPath[htmlFileName])
			 return this.html2SrcPath[htmlFileName];
		 else
			 return null;
	}
	this.getLinkFromRoot = function (htmlFileName) {
		 if (this.html2Root[htmlFileName])
			 return this.html2Root[htmlFileName];
		 else
			 return null;
	}
}
Html2SrcLink.instance = new Html2SrcLink();
var fileList = [
"ert_main_cpp.html","Current_FOC_cpp.html","Current_FOC_h.html","Current_FOC_private_h.html","Current_FOC_types_h.html","rtwtypes_h.html","mw_cmsis_h.html"];
