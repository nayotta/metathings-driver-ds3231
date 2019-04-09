package main

import (
	"encoding/hex"
	"fmt"

	"github.com/wumansgy/goEncrypt"
)

func main() {
	plainText := []byte("1")

	//这里传入的私钥和公钥是要用GetECCKey里面得到的私钥和公钥，如果自己封装的话，
	// 获取密钥时传入的第一个参数是要用这条曲线elliptic.P256()，如果用别的会报无效公钥错误，
	// 例如用P521()这条曲线
	privateKey := []byte(`-----BEGIN WUMAN ECC PRIVATE KEY-----
MHcCAQEEIKozbXD9G6bGPJ26cCAfEdLrqAe697F8SiLRMdqxzNQ5oAoGCCqGSM49AwEHoUQDQgAEk3/hltyR0r0J2Wkkhi4HaREJXS1vFooGpsKCbLvrdUW4peVIwKEW+yC3/g2X7Q2A8ftJlYv2X4kDU180GhIQpA==
-----END WUMAN ECC PRIVATE KEY-----
`)
	publicKey := []byte(`-----BEGIN WUMAN ECC PUBLIC KEY-----
MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEk3/hltyR0r0J2Wkkhi4HaREJXS1vFooGpsKCbLvrdUW4peVIwKEW+yC3/g2X7Q2A8ftJlYv2X4kDU180GhIQpA==
-----END WUMAN ECC PUBLIC KEY-----
`)

	cryptText, _ := goEncrypt.EccPublicEncrypt(plainText, publicKey)
	fmt.Println("ECC传入公钥加密的密文为：", hex.EncodeToString(cryptText))

	msg, err := goEncrypt.EccPrivateDeCrypt(cryptText, privateKey)
	if err != nil {
		fmt.Println(err)
	}
	fmt.Println("ECC传入私钥解密后的明文为：", string(msg))

}
