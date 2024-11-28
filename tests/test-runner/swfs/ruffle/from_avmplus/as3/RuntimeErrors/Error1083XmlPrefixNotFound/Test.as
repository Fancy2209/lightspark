/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
package {
import flash.display.MovieClip; public class Test extends MovieClip {}
}

import com.adobe.test.Assert;
import com.adobe.test.Utils;
var CODE = 1083; // The prefix "_" for element "_" is not bound.

//-----------------------------------------------------------
//-----------------------------------------------------------

try {
    var result = "no error";
    x1 = <foo:x xmlns:test="test"/>;
} catch (err) {
    result = err.toString();
} finally {
    Assert.expectEq("Runtime Error", Utils.TYPEERROR + CODE, Utils.typeError(result));
}

//-----------------------------------------------------------
//-----------------------------------------------------------
