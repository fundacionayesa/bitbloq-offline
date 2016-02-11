'use strict';

/**
 * @ngdoc function
 * @name bitbloqOffline.controller:BloqsProjectCtrl
 * @description
 * # BloqsProjectCtrl
 * Controller of the bitbloqOffline
 */
angular.module('bitbloqOffline')
  .controller('BloqsProjectCtrl', function($scope, $rootScope, bloqsUtils, common, _, $log, bloqs) {
    console.log('bloqsproject ctrl', $scope.$parent.$id);

    $scope.setProject = function(project) {
      $scope.project = project;
    }
    $scope.updateBloqs = function() {
      console.log('updating bloqs')
      if ($scope.arduinoMainBloqs.varsBloq) {

        var allBloqs = bloqs.bloqs;
        var allComponents = [];

        //Why?
        for (var bloq in allBloqs) {
          allBloqs[bloq].componentsArray = $scope.componentsArray;
        }

        var updateBloq = function(element, list) {

          var tempValue,
            tempRef;

          tempRef = element.dataset.reference;
          tempValue = element.dataset.value;

          bloqsUtils.drawDropdownOptions($(element), list);

          if (tempRef && tempValue) {

            var componentRef = list.find(function(comp) {
              return comp.uid === tempRef;
            });

            if (componentRef) {
              element.value = componentRef.name;
              element.dataset.reference = componentRef.uid;
              element.dataset.value = componentRef.name;
            }

          } else {
            $log.debug('dropdown not selected');
            element.selectedIndex = 0;
          }

        };
        var bloqCanvasEl = null;
        //Update dropdowns values from bloqs canvas
        for (var type in $scope.componentsArray) {
          if (!$scope.componentsArray[type].length) {
            continue;
          }
          bloqCanvasEl = document.getElementsByClassName('bloqs-tab')[0];
          var nodeList = bloqCanvasEl.querySelectorAll('select[data-dropdowncontent="' + type + '"]');
          for (var i = 0, len = nodeList.length; i < len; i++) {
            updateBloq(nodeList[i], $scope.componentsArray[type]);
          }
          allComponents = allComponents.concat($scope.componentsArray[type]);
        }
        //Update dropdowns from bloqs of toolbox
        if (bloqCanvasEl) {
          var toolboxNodeList = bloqCanvasEl.querySelectorAll('select[data-dropdowncontent="varComponents"]');
          for (var j = 0, len2 = toolboxNodeList.length; j < len2; j++) {
            updateBloq(toolboxNodeList[j], allComponents, true);
          }
        }

      }
    };

    $scope.showComponents = function(item) {
      var stopWord = ['analogWrite', 'digitalWrite', 'hwVariable', 'pinReadAdvanced', 'pinWriteAdvanced', 'turnOnOffAdvanced', 'digitalReadAdvanced', 'analogReadAdvanced'];

      if (stopWord.indexOf(item) === -1) {
        var userComponents = _.keys(_.pick($scope.componentsArray, function(value) {
          return value.length > 0;
        }));
        var result = false;
        userComponents.forEach(function(value) {
          if (value[value.length - 1] === 's') {
            value = value.substring(0, value.length - 1);
          }
          if (value === 'servo') {
            value = 'servoNormal';
          }
          item = item.toUpperCase();
          value = value.toUpperCase();
          if (item.includes('RGBLED')) {
            if (value.includes('RGB')) {
              result = true;
            }
          } else {
            if (item.includes(value) || value.includes(item)) {
              result = true;
            }
          }
        });
        return result;
      } else {
        return true;
      }
    };

    $scope.refreshComponentsArray = function() {
      console.log('refreshComponentsArray');

      var newComponentsArray = bloqsUtils.getEmptyComponentsArray();
      var newHardwareTags = [];
      var readyToSave = false;

      var plainComponentListTemporal = [];
      var plainComponentList = [];

      $scope.project.hardware.components.forEach(function(comp) {
        if (!!comp.connected) {
          if (comp.oscillator === true || comp.oscillator === 'true') {
            newComponentsArray.oscillators.push(_.cloneDeep(comp));
          } else {
            newComponentsArray[comp.category].push(_.cloneDeep(comp));
          }
          plainComponentListTemporal.push({
            'uid': comp.uid,
            'name': comp.name
          });
          newHardwareTags.push(comp.id);
        }
      });



      $scope.project.hardwareTags = _.uniq(newHardwareTags); //Regenerate hw tags

      if ($scope.project.hardware.robot) {
        newComponentsArray.robot.push($scope.project.hardware.robot);
        $scope.project.hardwareTags.push($scope.project.hardware.robot);
      }

      if ($scope.project.hardware.board) {
        $scope.project.hardwareTags.push($scope.project.hardware.board);
      }

      if ($scope.componentsArray.robot.length > 0) {
        plainComponentList = $scope.componentsArray.robot;
      } else {
        _.forEach($scope.componentsArray, function(n, key) {
          var compUidList = _.map($scope.componentsArray[key], function(item) {
            return {
              'uid': item.uid,
              'name': item.name
            };
          });
          if (compUidList && compUidList.length > 0) {
            plainComponentList = plainComponentList.concat(compUidList);
          }
        });
      }

      //Has changed componentsArray?
      if (plainComponentListTemporal.length > 0 || (plainComponentList.length > 0 && plainComponentList.indexOf('zowi') === -1)) {
        if (!_.isEqual(plainComponentList, plainComponentListTemporal) && !$scope.hardware.firstLoad) {
          $log.debug('componentsArray has changed');
          readyToSave = true;
        }
      }

      $scope.componentsArray = newComponentsArray;
      $scope.updateBloqs();

      if (!$scope.hardware.firstLoad && readyToSave) {
        $scope.startAutosave();
      }

    };
    $scope.showCommunications = function(item) {
      var stopWord = ['convert'];
      if ($scope.componentsArray.serialElements) {
        return !(stopWord.indexOf(item) === -1 && $scope.componentsArray.serialElements.length === 0);
      } else {
        return false;
      }
    };
    $scope.getCurrentProject = function() {
      $scope.project.code = bloqsUtils.getCode($scope.componentsArray, $scope.arduinoMainBloqs);
      var project = _.cloneDeep($scope.project);
      if ($scope.arduinoMainBloqs.varsBloq) {
        project.software = {
          vars: $scope.arduinoMainBloqs.varsBloq.getBloqsStructure(),
          setup: $scope.arduinoMainBloqs.setupBloq.getBloqsStructure(),
          loop: $scope.arduinoMainBloqs.loopBloq.getBloqsStructure()
        };
      }
      //project.hardware = $scope.getHardwareSchema();
      return project;
    };


    $scope.componentsArray = bloqsUtils.getEmptyComponentsArray();
    $scope.arduinoMainBloqs = {
      varsBloq: null,
      setupBloq: null,
      loopBloq: null
    };

    $scope.hardware = {
      boardList: null,
      componentList: null,
      robotList: null,
      cleanSchema: null,
      firstLoad: true,
      sortToolbox: null
    };

    $scope.project = {
      creatorId: '',
      name: common.translate('new-project'),
      description: '',
      userTags: [],
      hardwareTags: [],
      compiled: false,
      imageType: '',
      videoUrl: '',
      defaultTheme: 'infotab_option_colorTheme',
      software: {
        vars: {
          enable: true,
          name: 'varsBloq',
          childs: [],
          content: [
            []
          ]
        },
        setup: {
          enable: true,
          name: 'setupBloq',
          childs: [],
          content: [
            []
          ]
        },
        loop: {
          enable: true,
          name: 'loopBloq',
          childs: [],
          content: [
            []
          ]
        },
        freeBloqs: []
      },

      hardware: {
        board: null,
        robot: null,
        components: [],
        connections: []
      }
    };
  });