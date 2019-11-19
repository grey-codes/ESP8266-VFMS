function result() {
		let calcForm = document.forms["bigCalculator"];
        let inputx = calcForm.elements["x"];
		let inputy = calcForm.elements["y"];

      	window.alert(multiply(inputx, inputy));

}
function multiply(x, y) {

    const res = Array(x.length+y.length).fill(0);

    for (let i = x.length; i--; null) {

        let carry = 0;

        for (let j = y.length; j--; null) {

            res[1+i+j] += carry + x[i]*y[j];
			
            carry = Math.floor(res[1+i+j] / 10);
			
            res[1+i+j] = res[1+i+j] % 10;

        }

        res[i] += carry;
    }

    return res.join("").replace(/^0*(\d)/, "$1");
}
