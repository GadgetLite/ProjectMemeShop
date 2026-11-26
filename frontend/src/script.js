/*Variables*/
const signUpbtn = document.getElementById("signuppg-btn");
const loginbtn = document.getElementById("loginpg-btn");
const homebtn = document.getElementById("homepg-btn");
const cartbtn = document.getElementById("cartpg-btn");
const loginForm = document.getElementById("login-form");
const signupForm = document.getElementById("signup-form");
const hometxt = document.getElementById("home-txt");
const loginSubBtn = document.getElementById("loginSub-btn");
const signupSubBtn = document.getElementById("signupSub-btn");
const memeList = document.getElementById("meme-list");
const headBar = document.querySelector(".bar");
const addCartbtn = document.getElementById("addToCart-btn");
const uploadpgbtn = document.getElementById("uploadpg-btn");
const dropArea = document.getElementById("drop-area");
const memeImg = document.getElementById("meme-img");
const imgView = document.getElementById("img-view");
const uploadForm = document.getElementById("upload-form");



/*Uploading memes*/
memeImg?.addEventListener("change", uploadImage);

function uploadImage() {
    let imgLink = URL.createObjectURL(memeImg.files[0]);
    imgView.style.backgroundImage = `url(${imgLink})`;
    imgView.textContent = "";
    imgView.style.border = 0;
}

dropArea?.addEventListener("dragover", function(e) {
    e.preventDefault();
});

dropArea?.addEventListener("drop", function(e) {
    e.preventDefault();
    memeImg.files = e.dataTransfer.files;
    uploadImage();
});




let isLoggedIn = false;

if(isLoggedIn == true) {
    headBar.style.color = "red";
}


/*Creates front page list of memes*/
if(memeList) {
    for(let i = 0; i < 20; i++) {
        const newMeme = document.createElement("div");
        
        newMeme.classList.add("meme");
        newMeme.textContent = `Meme ${i + 1}`;

        memeList.appendChild(newMeme);
    }
}



/*Checks when buttons are pushed*/
hometxt?.addEventListener("click", () => {
    console.log("home");
}); 

homebtn?.addEventListener("click", () => {
    console.log("home");
});

signUpbtn?.addEventListener("click", () => {
    console.log("signup");
});

loginbtn?.addEventListener("click", () => {
    console.log("login");
});

cartbtn?.addEventListener("click", () => {
    console.log("cart");
});

addCartbtn?.addEventListener("click", () => {
    console.log("added meme to cart");
});
uploadpgbtn?.addEventListener("click", () => {
    console.log("upload pg");
});



if (uploadForm) {
    uploadForm.addEventListener("submit", async (event) => {
        event.preventDefault();

        const memeImg = document.getElementById("meme-img");
        const title = document.getElementById("title-input").value.trim();
        const price = document.getElementById("price-input");
        const errorEl = document.getElementById("err");
        errorEl.textContent = "";

        try {
            const response = await fetch("http://localhost:8080/api/login", {
                method: "POST",
                headers: {"Content-Type": "application/json"},
                body: JSON.stringify({memeImg, title, price})
            });

            console.log(response.status, response.url);
            const text = await response.text();
            console.log("raw body:", text);
       
            let data = {};
            try {
                data = text ? JSON.parse(text) : {};
            }
            catch {
                console.error("Not valid JSON:", text);
            }

            console.log("form exists");
            isLoggedIn = true;

            if(!response.ok) {
                errorEl.textContent = data.message || "Upload failed";
                return;
            }

            localStorage.setItem("token", data.token);
            window.location.href = "/";
            
        } catch (err) {
            errorEl.textContent = "Could not reach the server";
            console.error(err);
        }
    });
}


/*Creates post request for login form*/
if (loginForm) {
    loginForm.addEventListener("submit", async (event) => {
        event.preventDefault();

        const username = document.getElementById("username-input").value.trim();
        const password = document.getElementById("password-input").value;
        const errorEl = document.getElementById("err");
        errorEl.textContent = "";

        try {
            const response = await fetch("http://localhost:8080/api/login", {
                method: "POST",
                headers: {"Content-Type": "application/json"},
                body: JSON.stringify({username, password})
            });

            console.log(response.status, response.url);
            const text = await response.text();
            console.log("raw body:", text);
       
            let data = {};
            try {
                data = text ? JSON.parse(text) : {};
            }
            catch {
                console.error("Not valid JSON:", text);
            }

            console.log("form exists");
            isLoggedIn = true;

            if(!response.ok) {
                errorEl.textContent = data.message || "Login failed";
                return;
            }

            localStorage.setItem("token", data.token);
            window.location.href = "/";
            
        } catch (err) {
            errorEl.textContent = "Could not reach the server";
            console.error(err);
        }
    });
}



/*Creates post request for sign up form*/
if (signupForm) {
    signupForm.addEventListener("submit", async (event) => {  
        event.preventDefault();

        console.log("form exists");

        const firstName = document.getElementById("first-name-input").value.trim();
        const lastName = document.getElementById("last-name-input").value.trim();
        const username = document.getElementById("username-input").value.trim();
        const password = document.getElementById("password-input").value;
        const passwordRe = document.getElementById("password-re-input").value;
        const errorEl = document.getElementById("err");
        errorEl.textContent = "";

        if (password !== passwordRe) {
            errorEl.textContent = "Passwords do not match";
            return;
        }

        try {
            const response = await fetch("http://localhost:8080/api/signup", {
                method: "POST",
                headers: {"Content-Type": "application/json"},
                body: JSON.stringify({username, password, firstName, lastName})
            });
            
            console.log(response.status, response.url);
            const text = await response.text();
            console.log("raw body:", text);
       
            let data = {};
            try {
                data = text ? JSON.parse(text) : {};
            } catch {
                console.error("Not valid JSON:", text);
            }

            if(!response.ok) {
                errorEl.textContent = data.message || "Sign up failed";
                return;
            }

            localStorage.setItem("token", data.token);
            window.location.href = "/";
        } catch (err) {
            errorEl.textContent = "Could not reach the server";
            console.error(err);
        }
    });
}
