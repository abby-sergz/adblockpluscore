/*
 * This file is part of Adblock Plus <https://adblockplus.org/>,
 * Copyright (C) 2006-present eyeo GmbH
 *
 * Adblock Plus is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * Adblock Plus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Adblock Plus. If not, see <http://www.gnu.org/licenses/>.
 */

"use strict";

/**
 * Short from withNativeArgumentDeletion - wraps the function with the code
 * deleting native arguments at nativeArgPosition position(s).
 * Be careful, if an exception is thrown while construction of arguments, they
 * are not deleted.
 *
 * @param {(number|number[])} nativeArgPosition
 * @param {Function} fn - original function which should be wrapped
 * @param {Object=} thisObj - 'this' Object to which apply the function fn.
 * @return {Function} a new function object.
 */
exports.withNAD = function(nativeArgPosition, fn, thisObj)
{
  return function(...args)
  {
    try
    {
      fn.apply(thisObj ? thisObj : this, args);
    }
    finally
    {
      for (let i of Array.isArray(nativeArgPosition) ? nativeArgPosition : [nativeArgPosition])
        if (args[i])
          args[i].delete();
    }
  };
};

/**
 * Compares only prototype properties of converted from C++ objects.
 *
 * @param {Object} test - test
 * @param {Object} value - an inspecting object
 * @param {Object} expected - an expected object
 * @param {Object=} specialPropertyTesters - a dictionary with entries
 *        specifying a special testing function for properties which names are
 *        keys. The testing functions are corresponding values. If there is no
 *        such key in the dictionary then `test.equal` is used a the tester.
 * @param {Object=} valuePrototype - for internal usage. The prototype of
 *        the inspecting object from its inheritance chain, whose own
 *        properties have been already tested at the previous step. The
 *        function calls recursively itself in order to compare properties
 *        defined in all base prototypes.
 *        If the value is undefined then the parameter value is used.
 * @param {Object} expectedPrototype - for internal usage, see valuePrototype.
 */
function testEqualObjProperties(test, value, expected, specialPropertyTesters,
  valuePrototype, expectedPrototype)
{
  valuePrototype = Object.getPrototypeOf(valuePrototype ? valuePrototype : value);
  expectedPrototype = Object.getPrototypeOf(expectedPrototype ? expectedPrototype : expected);
  test.ok(valuePrototype === expectedPrototype, "Wrong inheritance chains, they are likely different objects");
  if (!valuePrototype)
    return;
  let propDescriptions = Object.getOwnPropertyDescriptors(expectedPrototype);
  for (let propName in propDescriptions)
    if ("get" in propDescriptions[propName])
      ((specialPropertyTesters && propName in specialPropertyTesters) ?
        specialPropertyTesters[propName] : test.equal)(value[propName], expected[propName], "Property: " + propName);
  testEqualObjProperties(test, value, expected, specialPropertyTesters, valuePrototype, expectedPrototype);
}
exports.testEqualObjProperties = testEqualObjProperties;
