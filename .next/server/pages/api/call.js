module.exports =
/******/ (function(modules) { // webpackBootstrap
/******/ 	// The module cache
/******/ 	var installedModules = require('../../ssr-module-cache.js');
/******/
/******/ 	// The require function
/******/ 	function __webpack_require__(moduleId) {
/******/
/******/ 		// Check if module is in cache
/******/ 		if(installedModules[moduleId]) {
/******/ 			return installedModules[moduleId].exports;
/******/ 		}
/******/ 		// Create a new module (and put it into the cache)
/******/ 		var module = installedModules[moduleId] = {
/******/ 			i: moduleId,
/******/ 			l: false,
/******/ 			exports: {}
/******/ 		};
/******/
/******/ 		// Execute the module function
/******/ 		var threw = true;
/******/ 		try {
/******/ 			modules[moduleId].call(module.exports, module, module.exports, __webpack_require__);
/******/ 			threw = false;
/******/ 		} finally {
/******/ 			if(threw) delete installedModules[moduleId];
/******/ 		}
/******/
/******/ 		// Flag the module as loaded
/******/ 		module.l = true;
/******/
/******/ 		// Return the exports of the module
/******/ 		return module.exports;
/******/ 	}
/******/
/******/
/******/ 	// expose the modules object (__webpack_modules__)
/******/ 	__webpack_require__.m = modules;
/******/
/******/ 	// expose the module cache
/******/ 	__webpack_require__.c = installedModules;
/******/
/******/ 	// define getter function for harmony exports
/******/ 	__webpack_require__.d = function(exports, name, getter) {
/******/ 		if(!__webpack_require__.o(exports, name)) {
/******/ 			Object.defineProperty(exports, name, { enumerable: true, get: getter });
/******/ 		}
/******/ 	};
/******/
/******/ 	// define __esModule on exports
/******/ 	__webpack_require__.r = function(exports) {
/******/ 		if(typeof Symbol !== 'undefined' && Symbol.toStringTag) {
/******/ 			Object.defineProperty(exports, Symbol.toStringTag, { value: 'Module' });
/******/ 		}
/******/ 		Object.defineProperty(exports, '__esModule', { value: true });
/******/ 	};
/******/
/******/ 	// create a fake namespace object
/******/ 	// mode & 1: value is a module id, require it
/******/ 	// mode & 2: merge all properties of value into the ns
/******/ 	// mode & 4: return value when already ns object
/******/ 	// mode & 8|1: behave like require
/******/ 	__webpack_require__.t = function(value, mode) {
/******/ 		if(mode & 1) value = __webpack_require__(value);
/******/ 		if(mode & 8) return value;
/******/ 		if((mode & 4) && typeof value === 'object' && value && value.__esModule) return value;
/******/ 		var ns = Object.create(null);
/******/ 		__webpack_require__.r(ns);
/******/ 		Object.defineProperty(ns, 'default', { enumerable: true, value: value });
/******/ 		if(mode & 2 && typeof value != 'string') for(var key in value) __webpack_require__.d(ns, key, function(key) { return value[key]; }.bind(null, key));
/******/ 		return ns;
/******/ 	};
/******/
/******/ 	// getDefaultExport function for compatibility with non-harmony modules
/******/ 	__webpack_require__.n = function(module) {
/******/ 		var getter = module && module.__esModule ?
/******/ 			function getDefault() { return module['default']; } :
/******/ 			function getModuleExports() { return module; };
/******/ 		__webpack_require__.d(getter, 'a', getter);
/******/ 		return getter;
/******/ 	};
/******/
/******/ 	// Object.prototype.hasOwnProperty.call
/******/ 	__webpack_require__.o = function(object, property) { return Object.prototype.hasOwnProperty.call(object, property); };
/******/
/******/ 	// __webpack_public_path__
/******/ 	__webpack_require__.p = "";
/******/
/******/
/******/ 	// Load entry module and return exports
/******/ 	return __webpack_require__(__webpack_require__.s = "./pages/api/call.js");
/******/ })
/************************************************************************/
/******/ ({

/***/ "./pages/api/call.js":
/*!***************************!*\
  !*** ./pages/api/call.js ***!
  \***************************/
/*! exports provided: default */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
eval("__webpack_require__.r(__webpack_exports__);\n/* harmony export (binding) */ __webpack_require__.d(__webpack_exports__, \"default\", function() { return handler; });\nconst {\n  execFile\n} = __webpack_require__(/*! child_process */ \"child_process\");\n\nconst path = __webpack_require__(/*! path */ \"path\");\n\nfunction handler(req, res) {\n  let args = [];\n\n  if (req.query && req.query.args) {\n    args = JSON.parse(req.query.args);\n  }\n\n  const binary = path.resolve(process.cwd(), \"elf_parser\");\n  execFile(binary, [...args], (error, stdout, stderr) => {\n    if (error) {\n      throw error;\n    }\n\n    res.status(200).send(stdout);\n  });\n}//# sourceURL=[module]\n//# sourceMappingURL=data:application/json;charset=utf-8;base64,eyJ2ZXJzaW9uIjozLCJzb3VyY2VzIjpbIndlYnBhY2s6Ly8vLi9wYWdlcy9hcGkvY2FsbC5qcz8wOWNlIl0sIm5hbWVzIjpbImV4ZWNGaWxlIiwicmVxdWlyZSIsInBhdGgiLCJoYW5kbGVyIiwicmVxIiwicmVzIiwiYXJncyIsInF1ZXJ5IiwiSlNPTiIsInBhcnNlIiwiYmluYXJ5IiwicmVzb2x2ZSIsInByb2Nlc3MiLCJjd2QiLCJlcnJvciIsInN0ZG91dCIsInN0ZGVyciIsInN0YXR1cyIsInNlbmQiXSwibWFwcGluZ3MiOiJBQUFBO0FBQUE7QUFBQSxNQUFNO0FBQUVBO0FBQUYsSUFBZUMsbUJBQU8sQ0FBQyxvQ0FBRCxDQUE1Qjs7QUFDQSxNQUFNQyxJQUFJLEdBQUdELG1CQUFPLENBQUMsa0JBQUQsQ0FBcEI7O0FBRWUsU0FBU0UsT0FBVCxDQUFpQkMsR0FBakIsRUFBc0JDLEdBQXRCLEVBQTJCO0FBQ3hDLE1BQUlDLElBQUksR0FBRyxFQUFYOztBQUNBLE1BQUlGLEdBQUcsQ0FBQ0csS0FBSixJQUFhSCxHQUFHLENBQUNHLEtBQUosQ0FBVUQsSUFBM0IsRUFBaUM7QUFDL0JBLFFBQUksR0FBR0UsSUFBSSxDQUFDQyxLQUFMLENBQVdMLEdBQUcsQ0FBQ0csS0FBSixDQUFVRCxJQUFyQixDQUFQO0FBQ0Q7O0FBRUQsUUFBTUksTUFBTSxHQUFHUixJQUFJLENBQUNTLE9BQUwsQ0FBYUMsT0FBTyxDQUFDQyxHQUFSLEVBQWIsRUFBNEIsWUFBNUIsQ0FBZjtBQUNBYixVQUFRLENBQUNVLE1BQUQsRUFBUyxDQUFDLEdBQUdKLElBQUosQ0FBVCxFQUFvQixDQUFDUSxLQUFELEVBQVFDLE1BQVIsRUFBZ0JDLE1BQWhCLEtBQTJCO0FBQ3JELFFBQUlGLEtBQUosRUFBVztBQUNULFlBQU1BLEtBQU47QUFDRDs7QUFDRFQsT0FBRyxDQUFDWSxNQUFKLENBQVcsR0FBWCxFQUFnQkMsSUFBaEIsQ0FBcUJILE1BQXJCO0FBQ0QsR0FMTyxDQUFSO0FBTUQiLCJmaWxlIjoiLi9wYWdlcy9hcGkvY2FsbC5qcy5qcyIsInNvdXJjZXNDb250ZW50IjpbImNvbnN0IHsgZXhlY0ZpbGUgfSA9IHJlcXVpcmUoXCJjaGlsZF9wcm9jZXNzXCIpO1xuY29uc3QgcGF0aCA9IHJlcXVpcmUoXCJwYXRoXCIpO1xuXG5leHBvcnQgZGVmYXVsdCBmdW5jdGlvbiBoYW5kbGVyKHJlcSwgcmVzKSB7XG4gIGxldCBhcmdzID0gW107XG4gIGlmIChyZXEucXVlcnkgJiYgcmVxLnF1ZXJ5LmFyZ3MpIHtcbiAgICBhcmdzID0gSlNPTi5wYXJzZShyZXEucXVlcnkuYXJncyk7XG4gIH1cblxuICBjb25zdCBiaW5hcnkgPSBwYXRoLnJlc29sdmUocHJvY2Vzcy5jd2QoKSwgXCJlbGZfcGFyc2VyXCIpO1xuICBleGVjRmlsZShiaW5hcnksIFsuLi5hcmdzXSwgKGVycm9yLCBzdGRvdXQsIHN0ZGVycikgPT4ge1xuICAgIGlmIChlcnJvcikge1xuICAgICAgdGhyb3cgZXJyb3I7XG4gICAgfVxuICAgIHJlcy5zdGF0dXMoMjAwKS5zZW5kKHN0ZG91dCk7XG4gIH0pO1xufVxuIl0sInNvdXJjZVJvb3QiOiIifQ==\n//# sourceURL=webpack-internal:///./pages/api/call.js\n");

/***/ }),

/***/ "child_process":
/*!********************************!*\
  !*** external "child_process" ***!
  \********************************/
/*! no static exports found */
/***/ (function(module, exports) {

eval("module.exports = require(\"child_process\");//# sourceURL=[module]\n//# sourceMappingURL=data:application/json;charset=utf-8;base64,eyJ2ZXJzaW9uIjozLCJzb3VyY2VzIjpbIndlYnBhY2s6Ly8vZXh0ZXJuYWwgXCJjaGlsZF9wcm9jZXNzXCI/M2RhNyJdLCJuYW1lcyI6W10sIm1hcHBpbmdzIjoiQUFBQSIsImZpbGUiOiJjaGlsZF9wcm9jZXNzLmpzIiwic291cmNlc0NvbnRlbnQiOlsibW9kdWxlLmV4cG9ydHMgPSByZXF1aXJlKFwiY2hpbGRfcHJvY2Vzc1wiKTsiXSwic291cmNlUm9vdCI6IiJ9\n//# sourceURL=webpack-internal:///child_process\n");

/***/ }),

/***/ "path":
/*!***********************!*\
  !*** external "path" ***!
  \***********************/
/*! no static exports found */
/***/ (function(module, exports) {

eval("module.exports = require(\"path\");//# sourceURL=[module]\n//# sourceMappingURL=data:application/json;charset=utf-8;base64,eyJ2ZXJzaW9uIjozLCJzb3VyY2VzIjpbIndlYnBhY2s6Ly8vZXh0ZXJuYWwgXCJwYXRoXCI/NzRiYiJdLCJuYW1lcyI6W10sIm1hcHBpbmdzIjoiQUFBQSIsImZpbGUiOiJwYXRoLmpzIiwic291cmNlc0NvbnRlbnQiOlsibW9kdWxlLmV4cG9ydHMgPSByZXF1aXJlKFwicGF0aFwiKTsiXSwic291cmNlUm9vdCI6IiJ9\n//# sourceURL=webpack-internal:///path\n");

/***/ })

/******/ });